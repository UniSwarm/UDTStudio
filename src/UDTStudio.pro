TEMPLATE = subdirs

SUBDIRS += \
    lib/udtgui \
    lib/udtgui/canFrameView \
    lib/udtgui/canSettingDialog \
    lib/od \
    lib/canopen \
    tools/cood \
    tools/ubl \
    udtstudio

cood.depends = od
udtgui.depends = od canopen
#udtstudio.depends = od canopen udtgui
canopen.depends = od
ubl.depends = canSettingDialog
