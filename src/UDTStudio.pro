TEMPLATE = subdirs

SUBDIRS += \
    od \
    canopen \
    udtgui \
    udtstudio

udtgui.depends = od canopen
udtstudio.depends = od canopen udtgui
