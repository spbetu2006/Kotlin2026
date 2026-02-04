#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 3)
        return 1;

    const size_t sz = atoi(argv[1]);

    srand(time({}));

    ofstream out(argv[2]);

    for (size_t i{};i<sz;++i) {
        out << rand()%100 << "," << rand()%100 << endl;
    }

    return 0;
}
