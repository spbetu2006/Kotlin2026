#include <stdio.h>
#include <stdlib.h>

/*
** Поддерживается одна из следующих архитектур
*/
// #define ARCH_X64 1
// #define ARCH_X32 1
// #define ARCH_X16 1

#ifdef ARCH_X64
    /*
    ** В зависимости от архитектуры определяется
    **     - размерность переменной
    **     - точность вычислений с фиксированной точкой
    ** NOTE: "fn_" префикс переменных "fixed number"
    */
    typedef int64_t          tNumber;
    typedef uint64_t tNumberUnsigned;
    const size_t tNumberSize = sizeof(tNumberUnsigned);
    const tNumber fixedNshift = 281474976710656; /* 2^48 */
    const char * s_dprintf = "%lld";
    const char * s_uprintf = "%llu";
    const tNumber fixed_epsilon = 16;

#elif ARCH_X32
    typedef int32_t          tNumber;
    typedef uint32_t tNumberUnsigned;
    const size_t tNumberSize = sizeof(tNumberUnsigned);
    const tNumber fixedNshift = 65536; /* 2^16 */
    const char * s_dprintf = "%ld";
    const char * s_uprintf = "%lu";
    const tNumber fixed_epsilon = 0;

#elif ARCH_X16
    typedef int16_t          tNumber;
    typedef uint16_t tNumberUnsigned;
    const size_t tNumberSize = sizeof(tNumberUnsigned);
    const tNumber fixedNshift = 1; /* 2^0 */
    const char * s_dprintf = "%d";
    const char * s_uprintf = "%u";
    const tNumber fixed_epsilon = 3;

#else
#error "Unsupported CPU arch"
#endif

#define MAX(A,B) (((A)>(B))?(A):(B))
#define MIN(A,B) (((A)>(B))?(B):(A))

const size_t gTableElemSize = 16;
const char * gFileTableName = "sq_table.tbl";
const char * gAccuracyInfo = "(при повышенной точности решение может быть лучше!)";

// Входные данные задачи (фиксированные)
#define gMaxPoints 100*100
const unsigned int gMaxDx = 99;
const unsigned int gMaxDy = 99;
const unsigned int gMaxRadius = 71;
const unsigned int gCenterX = 49;
const unsigned int gCenterY = 49;

// Вывод числа с фиксированной точкой (c префиксом "fn_")
void printfFixedNumber(tNumber fn_val) {
    if (fn_val < 0) {
       printf("-");
       fn_val = -fn_val;
    }
    printf(s_dprintf, (tNumber)(fn_val / fixedNshift));
    tNumber below = fn_val % fixedNshift; 
    if (below) {
        printf("%s", ".");
        while (below) {
            below *= 10;
            printf(s_dprintf, (tNumber)(below / fixedNshift));
            below %= fixedNshift;
        }
    } 
}

void readTableNumber(FILE *file, tNumberUnsigned* fn_val) {
     tNumberUnsigned _tmp = 1;
     unsigned char isLittleEndian = ( *((char*)&(_tmp)) == 1 );     
     // Считываем LittleEndian в LittleEndian   
     if (isLittleEndian) {
         char * ch = ((char*)fn_val)+tNumberSize-1;
         for (size_t i=0;i<tNumberSize;++i) {
             fread(ch, 1, 1, file);
             --ch;
         }
     } else 
     // Считываем с конвертацией LittleEndian в BigEndian
     {
         char * ch = ((char*)fn_val);
         for (size_t i=0;i<tNumberSize;++i) {
             fread(ch, 1, 1, file);
             ++ch;
         }
     }
     // "холостое" считывание разрядов точности, не "влезающих" в разрядность процессора
     char tmp;
     for (size_t i=0;i<(8-tNumberSize);++i) {
          fread(&tmp, 1, 1, file);
     }
}

unsigned char readTable(unsigned int dx
                       , unsigned int dy
                       , unsigned int R
                       , tNumberUnsigned* fn_delta_x
                       , tNumberUnsigned* fn_delta_y) {

    static FILE *table = NULL;
    if (!table) { 
        table = fopen(gFileTableName, "rb");
        // Отключение буферизации
        setvbuf(table, NULL, _IONBF, 0);
    }

    unsigned char bSwap = 0;
    // Swap: особенности размещения таблицы в файле - храним только упорядоченные пары (меньший, больший)
    if (dy < dx) {
        bSwap = 1;
        unsigned int tmp = dx;
        dx = dy;
        dy = tmp; 
    }
    size_t shift = 0;
    // NB: sqrt не используем, итерируемся :) 
    for (unsigned int i=0; i<=gMaxDx; ++i)
    for (unsigned int j=(i?i:1); j<=gMaxDy; ++j)
    for (unsigned int k=1; k<=gMaxRadius; ++k)
    {
        if (i*i + j*j > k*k) {
            continue;
        }       
        // Нашли табличный элемент - считываем
        if (i == dx && j == dy && k == R) {
            fseek(table, shift, SEEK_SET);
            if (bSwap) {
                readTableNumber(table, fn_delta_y);
                readTableNumber(table, fn_delta_x);
            } else {
                readTableNumber(table, fn_delta_x);
                readTableNumber(table, fn_delta_y);
            }
            return 0;
        }       
        shift += gTableElemSize;
    }
    return 1;
}

struct Point {
     unsigned char x, y;
};

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("%s", "Usage: ./nuke <targets_input> <radius>\n");
        return 1;
    }

    /*
    ** Считываем входные данные: радиус поражения и массив целей
    */
    const size_t radius = atoi(argv[2]);
    size_t n_points = 0;
    struct Point points[gMaxPoints];
    FILE * fCoords = fopen(argv[1], "r");
    if (!fCoords) {
         return 1;
    }
    while (n_points < gMaxPoints) {
         int px, py;
         if (2 == fscanf(fCoords, " %d , %d ", &px, &py)) {
             points[n_points].x = px;
             points[n_points].y = py;
             ++n_points;
         } else
             break;
    }
    fclose(fCoords);

    // Частный случай: "полное" решение
    if (radius >= gMaxRadius) {
        printf("%d %d %lu\n", gCenterX, gCenterY, n_points);
        return 0;
    }  

    // Частные случай: "простое" решение
    if (1 == n_points) {
        printf("%d %d %d\n", points[0].x, points[0].y, 1);
        return 0;
    }

    /*
    ** Общий случай (калькуляция решения):
    **     - перебираем возможные центры круга поражения
    **     - для каждого варианта считаем пораженные цели
    **     - находим максимум
    **     - выводим решение
    ** NOTE: используется стандартный O(N^3) алгоритм
    ** NOTE: используем арифметику с фиксированной точкой (без сопроцессора и float/double)
    */
    const tNumber radius2 = radius*radius;
    const tNumber fn_radius2 = radius2*fixedNshift;

    size_t max_targets = 0;
    tNumber fn_max_targets_x, fn_max_targets_y;

    for (size_t i=0;i<n_points;++i)
    for (size_t j=i+1;j<n_points;++j)
    {
        const tNumber dx = (tNumber)points[j].x - (tNumber)points[i].x; 
        const tNumber dy = (tNumber)points[i].y - (tNumber)points[j].y;

        // Вырожденный случай: расcтояние между точками i, j превышает заданный радиус
        if (dx*dx + dy*dy > radius2) {
            continue;
        } 

        tNumberUnsigned fn_delta_x, fn_delta_y;
        // Находим центр окружности заданного радиуса, проходящий через точки i и j (используем таблицу)
        if (!readTable(llabs(dx), llabs(dy), radius, &fn_delta_x, &fn_delta_y)) {

            const tNumber fn_delta_x2 = dx>0 ? fn_delta_x : -fn_delta_x;
            const tNumber fn_delta_y2 = dy>0 ? fn_delta_y : -fn_delta_y;
            const tNumber fn_cx = ((points[j].x + points[i].x)*fixedNshift)>>1;
            const tNumber fn_cy = ((points[j].y + points[i].y)*fixedNshift)>>1;

            for (size_t v=0;v<2;++v)
            {
                // Центр окружности: (center_x, center_y)
                const tNumber fn_center_x_delta = (v?fn_delta_x2:(-fn_delta_x2));
                const tNumber fn_center_y_delta = (v?fn_delta_y2:(-fn_delta_y2));
                const tNumber fn_center_x = fn_cx + ((fn_center_x_delta>0)?MAX(fn_center_x_delta-fixed_epsilon,0):MIN(fn_center_x_delta+fixed_epsilon,0));
                const tNumber fn_center_y = fn_cy + ((fn_center_y_delta>0)?MAX(fn_center_y_delta-fixed_epsilon,0):MIN(fn_center_y_delta+fixed_epsilon,0));
                size_t cur_targets = 0;

                // Считаем точки, которые попадают внутрь заданного радиуса
                for (size_t k=0;k<n_points;++k) {
                    const tNumber fn_pt_x = (tNumber)points[k].x * fixedNshift;
                    const tNumber fn_pt_y = (tNumber)points[k].y * fixedNshift;
                    const tNumber fn_rdd = ((fn_pt_x - fn_center_x)*((fn_pt_x - fn_center_x)/fixedNshift)) + 
                                           ((fn_pt_y - fn_center_y)*((fn_pt_y - fn_center_y)/fixedNshift));
                    if (fn_rdd <= fn_radius2) {
                        ++cur_targets;    
                    }
                }

                // Обновляем максимум при необходимости 
                if (cur_targets > max_targets) {
                    max_targets = cur_targets;
                    fn_max_targets_x = fn_center_x;
                    fn_max_targets_y = fn_center_y;
                }
            }
        }
    }

    if (max_targets) {
        printfFixedNumber(fn_max_targets_x);
        printf(" ");
        printfFixedNumber(fn_max_targets_y);
        printf(" %lu %s\n", max_targets, gAccuracyInfo);
    } else {
        /* Случай, когда "хорошего" решения не нашлось - выводим любую цель */
        printf("%d %d 1\n", points[0].x, points[0].y);
        return 0;
    }

    return 0;
}
