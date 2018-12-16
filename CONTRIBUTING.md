# Contribution "instructions"

## Getting the up-to-date source

git clone https://github.com/juzzlin/Heimer.git

- master branch has the currently released stuff and is stable

## Making changes

- Make your changes against the master branch

- If you have fixed a bug or modified the application in some other acceptable way,
I most likely want to receive the changes as a GitHub pull request

OR

- as a Git patch (make a commit of your changes in your working branch,
and then make a patch and send it via e-mail:

    `$ git diff master > your_change.patch`

- Please follow the existing coding style. Some examples and conventions below.

## Adding/updating translations

Qt's translation source files are at:

`src/translations/`

You can just copy these files as a new locale and edit, but the recommended way is
to generate/update the file from source code to make sure that the source strings
are up-to-date:

`$ lupdate src/*.cpp -ts src/translations/heimer_fr.ts`

..as an example for French.

The .ts-files can then be opened in Qt's translation GUI, linguist.

`$ linguist src/translations/heimer_fr.ts`

For Qt5, the Ubuntu package for these tools is `qttools5-dev-tools`.

After updating the strings, create the .qm file:

`$ lrelease src/translations/heimer_fr.ts -qm data/translations/heimer_fr.qm`

In the case of a new locale, the new .qm file needs to be added under `data/translations` and
`data/translations/translations.qrc` needs to be modified so that the new `.qm` file is included.

Remember to TEST the translations. Heimer needs to be rebuilt after a `.qm` file is updated.

Heimer can be forced to given lang (Finnish in the example):

`$ ./heimer --lang fi`

## Coding conventions

* Avoid useless comments and temp variables

* C++11 standard

* camelCase naming convention for members amd variables (m_-prefix for members)

* PascalCase naming convention for class names, namespaces, and enums

* Indentation with 4 spaces

* No copy-paste code

* No public/protected member variables allowed, only private variables with getters/setters

* Prefer references and smart pointers

* Use const always when possible

* Use auto

## Coding style examples

### Class header example:

```
#ifndef MYFOOCLASS_HPP
#define MYFOOCLASS_HPP

class MyFooClass : public MyBase
{
public:

    enum class MyEnum
    {
        This,
        That
    };

    MyFooClass();

    virtual ~MyFooClass();

    bool someMember() const;

    void setSomeMember(bool someMember);

protected:

    void myMethod1();

private:

    void myMethod2();

    bool m_someMember;

    int * m_somePointer;
};

typedef std::shared_ptr<MyFooClass> MyFooClassPtr;

#endif // MYFOOCLASS_HPP
```

### Class source example:

```
#include "myfooclass.hpp"

namespace {
static const int MAX_CARS = 10;
}

MyFooClass::MyFooClass()
    : m_someMember(false)
    , m_somePointer(nullptr)
{
    if (something())
    {
        for (int i = 0; i < MAX_CARS; i++)
        {
            doSomething(i);
        }
    }
}

bool MyFooClass::someMember() const
{
    return m_someMember;
}

void setSomeMember(bool someMember)
{
    m_someMember = someMember;
}

MyFooClass::~MyFooClass()
{
}
```

## Contact

E-mail: <jussi.lind@iki.fi>
