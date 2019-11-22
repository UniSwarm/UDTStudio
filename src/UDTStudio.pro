TEMPLATE = subdirs

SUBDIRS += \
    od \
    cood \
    canopen \
    udtgui \
    udtstudio

cood.depends = od
udtgui.depends = od canopen
udtstudio.depends = od canopen udtgui
