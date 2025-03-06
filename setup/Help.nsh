Function ShowHelp
	${IfNot} ${FileExists} "$PLUGINSDIR\help.chm"
		File "/ONAME=$PLUGINSDIR\help.chm" "..\help\obj\help.chm"
	${EndIf}

	Pop $0
	System::Call '${HtmlHelp}($HWNDPARENT, "$PLUGINSDIR\help.chm::/$0.html", ${HH_DISPLAY_TOPIC}, 0) .r0'
FunctionEnd

Function CloseHelp
	System::Call '${HtmlHelp}(0, 0, ${HH_CLOSE_ALL}, 0)'
FunctionEnd
