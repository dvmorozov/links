#include "View.h"

namespace Bookmarks
{
    class Page : public View
    {
    public:
        Page();
        ~Page();

        //  https://action.mindjet.com/task/14817423
        virtual void PrintScripts();
        //  https://action.mindjet.com/task/14817423
        virtual void PrintStyles();
        virtual void PrintHead(std::wstring title);
        virtual void PrintTail();

    protected:
        static void PrintInfo();
    };
}
