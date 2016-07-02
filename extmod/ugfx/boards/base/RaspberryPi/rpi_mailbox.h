#ifndef RPI_MAILBOX_H
#define RPI_MAILBOX_H

extern unsigned int readmailbox(unsigned int channel);
extern void writemailbox(unsigned int channel, unsigned int data);

#endif	/* RPI_MAILBOX_H */
