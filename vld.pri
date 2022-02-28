#CONFIG(debug, debug|release)
#{
#    DEFINES += $$(NSB_PROJECT_VLD)
#    CONFIG += $$(NSB_PROJECT_VLD)
#    NSB_PROJECT_VLD_ON
#    {
#        message(vld on! libs path: \"$$(NSB_PROJECT_VLD_LIBS)\" include path: \"$$(NSB_PROJECT_VLD_INCLUDE)\")
#        LIBS += -L"$$(NSB_PROJECT_VLD_LIBS)" -lvld
#        INCLUDEPATH += $$(NSB_PROJECT_VLD_INCLUDE)
#    }
#}
