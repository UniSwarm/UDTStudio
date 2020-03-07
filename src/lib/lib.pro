TEMPLATE = subdirs

SUBDIRS += \
    od \
    canopen \
    udtgui

# depends
canopen.depends = od
udtgui.depends = od canopen
