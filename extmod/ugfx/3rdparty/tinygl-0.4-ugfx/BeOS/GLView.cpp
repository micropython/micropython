#include "GLView.h"
#include <stdio.h>
#include <interface/Bitmap.h>

BLocker BGLView::locker;

BGLView::BGLView(BRect rect, char *name,
                 ulong resizingMode, ulong mode,
                 ulong options)
  : BView(rect, name, resizingMode, mode|B_FRAME_EVENTS|B_WILL_DRAW)
{
#ifdef __INTEL__
  color_space cs = B_RGB16_LITTLE;
#else
  color_space cs = B_RGB16_BIG;
#endif
  this->bitmaps[0] = new BBitmap(rect, cs, false, true);
  this->bitmaps[1] = new BBitmap(rect, cs, false, true);

  this->currBitmap = 0;
  int w = this->bitmaps[0]->BytesPerRow() / 2;
  int h = rect.Height() + 1;
  void *buffers[2];
  buffers[0] = this->bitmaps[0]->Bits();
  buffers[1] = this->bitmaps[1]->Bits();
  this->context = ostgl_create_context(w, h, 16, buffers, 2);
  ostgl_make_current(this->context, 0); 
}

BGLView::~BGLView()
{
  ostgl_delete_context(this->context);
  delete this->bitmaps[0];
  delete this->bitmaps[1];
}

void 
BGLView::LockGL()
{  
  BGLView::locker.Lock();
  ostgl_make_current(this->context, this->currBitmap);
}

void 
BGLView::UnlockGL()
{
  BGLView::locker.Unlock();
}

void 
BGLView::SwapBuffers()
{
  if (Window()->Lock()) {
    DrawBitmap(this->bitmaps[this->currBitmap]);
    Window()->Unlock();
    this->currBitmap ^= 1;
  }
}

/*
BView *
BGLView::EmbeddedView()
{
  return NULL;
}

status_t 
BGLView::CopyPixelsOut(BPoint source, BBitmap *dest)
{
  assert(0);
  return 0;
}

status_t 
BGLView::CopyPixelsIn(BBitmap *source, BPoint dest)
{
  assert(0);
  return 0;
}
*/

void 
BGLView::ErrorCallback(GLenum /*errorCode*/)
{
}

void 
BGLView::Draw(BRect rect)
{
  //fprintf(stderr, "GLView::Draw()");
  DrawBitmap(this->bitmaps[this->currBitmap^1], rect, rect);
}

void 
BGLView::AttachedToWindow()
{
}

void 
BGLView::AllAttached()
{
}

void 
BGLView::DetachedFromWindow()
{
}

void 
BGLView::AllDetached()
{
}

void 
BGLView::FrameResized(float w, float h)
{
  delete this->bitmaps[0];
  delete this->bitmaps[1];
#ifdef __INTEL__
  color_space cs = B_RGB16_LITTLE;
#else
  color_space cs = B_RGB16_BIG;
#endif
  this->bitmaps[0] = new BBitmap(BRect(0,0, w-1, h-1), 
                                 cs, false, true);
  this->bitmaps[1] = new BBitmap(BRect(0,0, w-1, h-1), 
                                 cs, false, true);
  int w2 = this->bitmaps[0]->BytesPerRow() / 2;
  void *buffers[2];
  buffers[0] = this->bitmaps[0]->Bits();
  buffers[1] = this->bitmaps[1]->Bits();
  ostgl_resize(this->context, w2, h, buffers);
}

/*
status_t 
BGLView::Perform(perform_code d, void *arg)
{
  
}
*/

//
// the rest are pass-through functions
//

status_t 
BGLView::Archive(BMessage *data, bool deep) const
{
  return BView::Archive(data, deep);
}

void 
BGLView::MessageReceived(BMessage *msg)
{
  BView::MessageReceived(msg);
}

void 
BGLView::SetResizingMode(uint32 mode)
{
  BView::SetResizingMode(mode);
}

void 
BGLView::Show()
{
  BView::Show();
}

void 
BGLView::Hide()
{
  BView::Hide();
}

BHandler *
BGLView::ResolveSpecifier(BMessage *msg, int32 index,
                          BMessage *specifier, int32 form,
					      const char *property)
{
  return BView::ResolveSpecifier(msg, index, specifier, form, property);
}

status_t 
BGLView::GetSupportedSuites(BMessage *data)
{
  return BView::GetSupportedSuites(data);
}

/*
void 
BGLView::DirectConnected( direct_buffer_info *info )
{
  BView::DirectConnected(info);
}
*/

/*
void 
BGLView::EnableDirectMode( bool enabled )
{
  BView::EnableDirectMode(enabled);
}
*/
