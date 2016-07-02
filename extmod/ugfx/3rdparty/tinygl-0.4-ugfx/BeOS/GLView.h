#ifndef _glview_h_
#define _glview_h_

#define BGL_RGB           0
#define BGL_INDEX         1
#define BGL_SINGLE        0
#define BGL_DOUBLE        2
#define BGL_DIRECT        0
#define BGL_INDIRECT      4
#define BGL_ACCUM         8
#define BGL_ALPHA        16
#define BGL_DEPTH        32
#define BGL_OVERLAY      64
#define BGL_UNDERLAY    128
#define BGL_STENCIL     512

#include <interface/View.h>
#include <support/Locker.h>
#include <GL/gl.h>
#include <GL/oscontext.h>
#include <game/WindowScreen.h>
#include <game/DirectWindow.h>

class BGLView : public BView {
public:
  BGLView(BRect rect, char *name,
          ulong resizingMode, ulong mode,
          ulong options);
  virtual ~BGLView();

  void LockGL();
  void UnlockGL();
  void SwapBuffers();
//  BView *EmbeddedView();
//  status_t CopyPixelsOut(BPoint source, BBitmap *dest);
//  status_t CopyPixelsIn(BBitmap *source, BPoint dest);

  virtual void ErrorCallback(GLenum errorCode);
  virtual void Draw(BRect updateRect);
  virtual void AttachedToWindow();
  virtual void AllAttached();
  virtual void DetachedFromWindow();
  virtual void AllDetached();
  virtual void FrameResized(float width, float height);
//  virtual status_t Perform(perform_code d, void *arg);

  //
  // Methods below are pass-throughs to BView for the moment.
  //
  
  virtual status_t Archive(BMessage *data, bool deep = true) const;
  virtual void MessageReceived(BMessage *msg);
  virtual void SetResizingMode(uint32 mode);

  virtual void Show();
  virtual void Hide();

  virtual BHandler *ResolveSpecifier(BMessage *msg, int32 index,
							         BMessage *specifier, int32 form,
							         const char *property);
  virtual status_t GetSupportedSuites(BMessage *data);
  //void DirectConnected( direct_buffer_info *info );
  //void EnableDirectMode( bool enabled );

private:
  ostgl_context *context;
  BBitmap *bitmaps[2];
  int currBitmap;
  static BLocker locker;
};

#endif // _glview_h_
