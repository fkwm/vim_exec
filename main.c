/*  
 *   set $VIM_PACKEGE_ROOT to this gvim.exe's dir
 *   set $HOME to $VIM_PACKEGE_ROOT/home
 *   read vimrc  from $VIM_PACKEGE_ROOT/data/settings/vimrc
 *   read gvimrc from $VIM_PACKEGE_ROOT/data/settings/gvimrc
 *     .... ignore default vimrc & gvimrc in original gvim.exe's dir
 *   attension: this wrapper program wait until original gvim.exe exit,
 *               so 'gvim.exe' process created two.
 */

#include <stdio.h>
#include <windows.h>
#include <mbstring.h>		// _ismbsleadÅA_ismbstrail

#define MAX_PATH_WITH_PARAMETER (MAX_PATH * 3)

/* this function free command_line */
void start_application(char * command_line) {
  HWND hWnd;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  CreateProcess(
    NULL,
    (LPTSTR)command_line,
    NULL,
    NULL,
    FALSE,
    CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE,
    NULL,
    NULL,
    &si,
    &pi
    );
  free(command_line);
  WaitForSingleObject( pi.hProcess, INFINITE ) ;
}

/* return module path (width trailing backslash) */
void
get_module_path(char * module_path)
{
  char module_file_name[ MAX_PATH ];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR] ;
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  int  dir_length;
  GetModuleFileName( NULL, module_file_name, MAX_PATH );
  _splitpath(module_file_name ,drive, dir, fname, ext);
  dir_length = strlen(dir);
  if(dir[dir_length - 1] == '\\' && !_ismbstrail(dir, &dir[dir_length - 1]))
    dir[dir_length - 1] = '\0';
  strcpy_s(module_path, MAX_PATH, drive);
  strcat_s(module_path, MAX_PATH, dir);
}

void
escape_path_backslash(char * buffer, const char * path)
{
  int i;
  int buffer_count =0;
  for(i = 0; i < strlen(path); i++) {
    buffer[buffer_count++] = path[i];
    if(path[i] == '\\' && !_ismbstrail(path, &path[i])) {
      buffer[buffer_count++] = path[i];
    }
  }
  buffer[buffer_count] = '\0';
}



void
get_use_vimrc_command_line(char * path, const char * module_path, const char * rc_name)
{
  if(strcmp(rc_name, "vimrc") == 0) {
    strcpy_s(path, MAX_PATH, " -u \"");
  } else {
    strcpy_s(path, MAX_PATH, " -U \"");
  }
  strcat_s(path, MAX_PATH, module_path);
  strcat_s(path, MAX_PATH, "\\data\\settings\\");
  strcat_s(path, MAX_PATH, rc_name);
  strcat_s(path, MAX_PATH, "\" ");
}

void
get_cmd_set_home_command_line(char * s, const char * module_path_escaped)
{
  strcpy_s(s, MAX_PATH, " --cmd \"");
  strcat_s(s, MAX_PATH, "let $HOME=\\\"");
  strcat_s(s, MAX_PATH, module_path_escaped);
  strcat_s(s, MAX_PATH, "\\\\home");
  strcat_s(s, MAX_PATH, "\\\"\" ");
}

void
get_cmd_set_vimpackegeroot_command_line(char * s, const char * module_path_escaped)
{
  strcpy_s(s, MAX_PATH, " --cmd \"");
  strcat_s(s, MAX_PATH, "let $VIM_PACKEGE_ROOT=\\\"");
  strcat_s(s, MAX_PATH, module_path_escaped);
  strcat_s(s, MAX_PATH, "\\\"\" ");
}

int
WINAPI WinMain(HINSTANCE hInstance ,
               HINSTANCE hPreInst ,
               LPSTR     lpszCmdLine,
               int       nCmdShow) {


  char module_path[MAX_PATH];
  char module_path_escaped[MAX_PATH];
  char vimrc_path_line[MAX_PATH];
  char gvimrc_path_line[MAX_PATH];
  char sethome_line[MAX_PATH];
  char setroot_line[MAX_PATH];
  char * command_line = NULL;
  int  command_line_size = 0;

  get_module_path(module_path);
  escape_path_backslash(module_path_escaped, module_path);

  get_use_vimrc_command_line(vimrc_path_line, module_path, "vimrc");
  get_use_vimrc_command_line(gvimrc_path_line, module_path, "gvimrc");
  get_cmd_set_home_command_line(sethome_line, module_path_escaped);
  get_cmd_set_vimpackegeroot_command_line(setroot_line, module_path_escaped);

  /* MessageBox(NULL, sethome_line, "vim", MB_OK); */

  command_line_size = MAX_PATH /* gvim.exe's path */
                               + strlen(vimrc_path_line)
                               + strlen(gvimrc_path_line)
                               + strlen(sethome_line)
                               + strlen(setroot_line)
                               + strlen(lpszCmdLine) /* command line from argument */
                               + 1 /* terminal null char */;

  command_line = (char*)malloc(command_line_size); /* free in start_application */

  strcpy_s(command_line, command_line_size, "\"");
  strcat_s(command_line, command_line_size, module_path);
  strcat_s(command_line, command_line_size, "\\vim\\gvim.exe\"");
  strcat_s(command_line, command_line_size, sethome_line);
  strcat_s(command_line, command_line_size, setroot_line);
  strcat_s(command_line, command_line_size, vimrc_path_line);
  strcat_s(command_line, command_line_size, gvimrc_path_line);
  strcat_s(command_line, command_line_size, lpszCmdLine);


  start_application(command_line);

  return 0;
}
