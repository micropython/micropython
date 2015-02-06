
#ifndef UPDATER_H_
#define UPDATER_H_

bool updater_check_path (void *path);
bool updater_start (void);
bool updater_write (uint8_t *buf, uint32_t len);
void updater_finnish (void);
bool updater_verify (uint8_t *rbuff, uint8_t *hasbuff);

#endif /* UPDATER_H_ */
