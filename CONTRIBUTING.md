# Contributing to UDTStudio

:+1: We are happy to learn that you want to contribute !

Thank you for making the embedded world a more attractive domain. Each commits make it simpler.

This little guide goals to help you to contribute to this project.

Before contributing, never forget the following points:

* Keep the lightweight and fast opening of the tool

## How Can I Contribute?

* Reporting bugs in [Issues](https://github.com/UniSwarm/UDTStudio/issues)

* Pull requests in [Pull requests](https://github.com/UniSwarm/UDTStudio/pulls)

## Styleguides

### Git commit messages

* Use past tense: `Updated function getSensor, with better scaling result`

* Include the part of project modified between square brackets.

  * `[project] msg` For a modification inside project model

* Never use message like : `Fixed bug` or `Updated project`

* Do only one modification per commit, not two differents bug fix

### C++ code styleguide

* no tab, only spaces, 4 spaces
* headers files with .h extension, .cpp for code files, no upper case in file name
* class name start with a capital and are in camel case style
* private and protected members start with an underscore and a lowercase, camel case style
* getters have the same name than the member without the underscore
* setters start by `set` followed by the member name with a capital

example, projectloader.h:
```C++
class ProjectLoader
{
public:
    ProjectLoader();

    const QString &globalPath() const;
    void setGlobalPath(const QString &globalPath);

protected:
    QString _globalPath;
};
```
