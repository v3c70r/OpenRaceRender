// Test 
#include "logreader.h"
#include <string>
#include <cassert>
int main(int argc, char** argv)
{
    LogReader reader(argv[1]);
    for (const auto& header: reader.GetHeaders())
    {
        assert(header.size() > 0);
    }
    return 0;
}
