#ifndef ERREUR_H_INCLUDED
#define ERREUR_H_INCLUDED

#include <exception>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT_HERE __FILE__ ":" TOSTRING(__LINE__)


using namespace std;

class CError: public exception
{
public:
    CError(string const& err="",string const& file="",string const& func="", int level=-1) throw()
    :m_err(err+string(" (in file ")+file+string(" ,in function: ")+func+string(" )\n")),m_level(level)
    {}

     virtual const char* what() const throw()
     {
         return m_err.c_str();
     }

     int getLevel() const throw()
     {
          return m_level;
     }

    virtual ~CError() throw()
    {}

private:
    string m_err;               // the error
    int m_level;               // level between 0 and 100 fate if it's 100
};


#define error(x,y) CError(x,AT_HERE,std::string(__FUNCTION__),y);

#endif // ERREUR_H_INCLUDED
