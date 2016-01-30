#ifndef UPDATE_H
#define UPDATE_H

//--------------------------------------------------------------------------
int update_ota_local(char *ip, int port);
int update_ota_cloud(char *repository, char *file_path);
void cmd_update(int argc, char **argv);

//----------------------------------------------------------------------------
#endif
