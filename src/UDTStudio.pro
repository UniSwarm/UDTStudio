TEMPLATE = subdirs

SUBDIRS += \
    lib \
    tools \
    udtstudio

# depends
tools.depends = lib
udtstudio.depends = lib

TRANSLATIONS = $$PWD/../translations/udtstudio_fr.ts \
               $$PWD/../translations/udtstudio_en.ts
