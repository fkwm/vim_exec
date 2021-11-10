# このMakefileは GNU MAKE 用です。
# you can download GNU MAKE for Windows from http://sourceforge.net/projects/unxutils

CC=cl -nologo
LD=link -nologo
LINK=link
RC=rc


all: \
	gvim.exe

CMNOBJ= \
	main.obj \
	resource.res

.SUFFIXES : 
.SUFFIXES : .obj .exe .c
.PRECIOUS : %.obj


CPPFLAGS=/GA /GL /I. /O1 /D_X86_ /D_WINDOWS
# /Tc ... C言語ファイル
# /01 ... 最適化
LIBS=
#LIBS= kernel32.lib \
#	user32.lib \
#	shell32.lib \
#	Comdlg32.lib     # for MessageBox function


LDFLAGS=/LTCG

%.exe: $(CMNOBJ)
	$(LD) $(LDFLAGS) -out:$@ $^ $(LIBS)

%.obj: %.c
	$(CC) $(CPPFLAGS) /c  /Tc $^

resource.res : resource.rc resource.h
	$(RC) /fo resource.res resource.rc

clean:
	rm *.obj *.exe *.res
