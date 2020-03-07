TEMPLATE = subdirs

SUBDIRS += \
    lib \
    tools \
    udtstudio

# depends
tools.depends = lib
udtstudio.depends = lib
