#include <iostream>
#include <stdexcept>

namespace std
{
class invalid_request : public runtime_error
{
public:
    invalid_request(const string& err_msg) : runtime_error(err_msg)
    {
    }
};

}  // End namespace
