/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */
#include "nsHTMLImageLoader.h"
#include "nsIHTMLReflow.h"
#include "nsFrame.h"
#include "nsIURL.h"
#include "nsNeckoUtil.h"

#ifdef DEBUG
#undef NOISY_IMAGE_LOADING
#else
#undef NOISY_IMAGE_LOADING
#endif

MOZ_DECL_CTOR_COUNTER(nsHTMLImageLoader);

nsHTMLImageLoader::nsHTMLImageLoader()
  : mBaseURL(nsnull),
    mFrame(nsnull),
    mCallBack(nsnull),
    mClosure(nsnull),
    mImageLoader(nsnull),
    mAllFlags(0),
    mIntrinsicImageSize(0, 0),
    mComputedImageSize(0, 0)
{
  MOZ_COUNT_CTOR(nsHTMLImageLoader);
}

nsHTMLImageLoader::~nsHTMLImageLoader()
{
  MOZ_COUNT_DTOR(nsHTMLImageLoader);
  NS_IF_RELEASE(mBaseURL);
  NS_IF_RELEASE(mImageLoader);
}

void
nsHTMLImageLoader::Init(nsIFrame* aFrame,
                        nsHTMLImageLoaderCB aCallBack,
                        void* aClosure,
                        nsIURI* aBaseURL,
                        const nsString& aURLSpec)
{
  mFrame = aFrame;
  mCallBack = aCallBack;
  mClosure = aClosure;
  mBaseURL = aBaseURL;
  NS_IF_ADDREF(mBaseURL);
  SetURL(aURLSpec);
}

nsIImage*
nsHTMLImageLoader::GetImage()
{
  nsIImage* image = nsnull;
  if (mImageLoader) {
    mImageLoader->GetImage(&image);
  }
  return image;
}

void
nsHTMLImageLoader::SetURL(const nsString& aNewSpec)
{
  mURLSpec = aNewSpec;
  if (mBaseURL && !aNewSpec.Equals("")) {
    nsString empty;
    nsresult rv;
    rv = NS_MakeAbsoluteURI(mURLSpec, mBaseURL, mURL);
    if (NS_FAILED(rv)) {
      mURL = mURLSpec;
    }
  } else {
    mURL = mURLSpec;
  }
}

void
nsHTMLImageLoader::StopLoadImage(nsIPresContext* aPresContext)
{
  if (mImageLoader) {
    aPresContext->StopLoadImage(mFrame, mImageLoader);
    NS_RELEASE(mImageLoader);
  }
}

void
nsHTMLImageLoader::StopAllLoadImages(nsIPresContext* aPresContext)
{
  aPresContext->StopAllLoadImagesFor(mFrame);
}

nsresult
nsHTMLImageLoader::ImageLoadCB(nsIPresContext* aPresContext,
                               nsIFrameImageLoader* aLoader,
                               nsIFrame* aFrame,
                               void* aClosure,
                               PRUint32 aStatus)
{
  if (aClosure) {
    ((nsHTMLImageLoader*)aClosure)->Update(aPresContext, aFrame, aStatus);
  }
  return NS_OK;
}

void
nsHTMLImageLoader::Update(nsIPresContext* aPresContext,
                          nsIFrame* aFrame,
                          PRUint32 aStatus)
{
#ifdef NOISY_IMAGE_LOADING
  nsFrame::ListTag(stdout, aFrame);
  printf(": update: status=%x [loader=%p] callBack=%p squelch=%s\n",
         aStatus, mImageLoader, mCallBack,
         mFlags.mSquelchCallback ? "yes" : "no");
#endif
  if (NS_IMAGE_LOAD_STATUS_SIZE_AVAILABLE & aStatus) {
    if (mImageLoader) {
      mImageLoader->GetSize(mIntrinsicImageSize);
      if (mFlags.mNeedIntrinsicImageSize) {
        mFlags.mHaveIntrinsicImageSize = PR_TRUE;
      }
    }
    if ((NS_IMAGE_LOAD_STATUS_SIZE_AVAILABLE == aStatus) &&
        !mFlags.mNeedSizeNotification) {
      return;
    }
  }

  // Pass on update to the user of this object if they want it
  if (mCallBack) {
    if (!mFlags.mSquelchCallback) {
      (*mCallBack)(aPresContext, this, aFrame, mClosure, aStatus);
    }
  }
}

// The prefix for special "internal" images that are well known
#define GOPHER_SPEC "internal-gopher-"

// Note: sizeof a string constant includes the \0 at the end so
// subtract one
#define GOPHER_SPEC_SIZE (sizeof(GOPHER_SPEC) - 1)

nsresult
nsHTMLImageLoader::StartLoadImage(nsIPresContext* aPresContext)
{
  if (!mFrame) {
    // We were not initialized!
    return NS_ERROR_NULL_POINTER;
  }
  if (mURL.Equals("")) {
    return NS_OK;
  }

  // Note: navigator 4.* and earlier releases ignored the base tags
  // effect on the builtin images. So we do too. Use mURLSpec instead
  // of the absolute url...
  nsAutoString internalImageURLSpec;
  nsString* urlSpec = &mURL;
  if (mURLSpec.Compare(GOPHER_SPEC, PR_FALSE, GOPHER_SPEC_SIZE) == 0) {
    // We found a special image source value that refers to a
    // builtin image. Rewrite the source url as a resource url.
    urlSpec = &internalImageURLSpec;
    mURLSpec.Mid(internalImageURLSpec, GOPHER_SPEC_SIZE,
                 mURLSpec.Length() - GOPHER_SPEC_SIZE);
    internalImageURLSpec.Insert("resource:/res/html/gopher-", 0);
    internalImageURLSpec.Append(".gif");
  }

  // This is kind of sick, but its possible that we will get a
  // notification *before* we have setup mImageLoader. To get around
  // this, we let the pres-context store into mImageLoader and sort
  // things after it returns.
  nsIFrameImageLoader* oldLoader = mImageLoader;
  nsSize* sizeToLoadWidth = nsnull;
  if (!mFlags.mAutoImageSize && !mFlags.mNeedIntrinsicImageSize) {
    sizeToLoadWidth = &mComputedImageSize;
  }
  nsresult rv = aPresContext->StartLoadImage(*urlSpec, nsnull,
                                             sizeToLoadWidth,
                                             mFrame, ImageLoadCB, (void*)this,
                                             &mImageLoader);
#ifdef NOISY_IMAGE_LOADING
  nsFrame::ListTag(stdout, mFrame);
  printf(": loading image '");
  fputs(mURL, stdout);
  printf("' @ ");
  if (mFlags.mNeedIntrinsicImageSize) {
    printf("intrinsic size ");
  }
  printf("%d,%d; oldLoader=%p newLoader=%p\n",
         mComputedImageSize.width, mComputedImageSize.height,
         oldLoader, mImageLoader);
#endif

  if (oldLoader != mImageLoader) {
    if (nsnull != oldLoader) {
      // Tell presentation context we are done with the old image loader
      aPresContext->StopLoadImage(mFrame, oldLoader);
    }
  }

  // Release the old image loader
  NS_IF_RELEASE(oldLoader);

  return rv;
}

void
nsHTMLImageLoader::UpdateURLSpec(nsIPresContext* aPresContext,
                                 const nsString& aNewSpec)
{
  SetURL(aNewSpec);

  // Start image loading with the previously computed size information
  StartLoadImage(aPresContext);
}

PRBool
nsHTMLImageLoader::GetDesiredSize(nsIPresContext* aPresContext,
                                  const nsHTMLReflowState* aReflowState,
                                  nsHTMLReflowMetrics& aDesiredSize)
{
  nscoord widthConstraint = NS_INTRINSICSIZE;
  nscoord heightConstraint = NS_INTRINSICSIZE;
  PRBool fixedContentWidth = PR_FALSE;
  PRBool fixedContentHeight = PR_FALSE;

  if (aReflowState) {
    widthConstraint = aReflowState->mComputedWidth;
    heightConstraint = aReflowState->mComputedHeight;

    // Determine whether the image has fixed content width and height
    if (NS_UNCONSTRAINEDSIZE != widthConstraint) {
      fixedContentWidth = PR_TRUE;
    }
    if (NS_UNCONSTRAINEDSIZE != heightConstraint) {
      fixedContentHeight = PR_TRUE;
    }
  }

  for (;;) {
    PRBool haveComputedSize = PR_FALSE;
    PRBool needIntrinsicImageSize = PR_FALSE;
    nscoord newWidth, newHeight;
    mFlags.mAutoImageSize = PR_FALSE;
    mFlags.mNeedSizeNotification = PR_FALSE;
    if (fixedContentWidth) {
      if (fixedContentHeight) {
        newWidth = widthConstraint;
        newHeight = heightConstraint;
        haveComputedSize = PR_TRUE;
      }
      else {
        // We have a width, and an auto height. Compute height from
        // width once we have the intrinsic image size.
        newWidth = widthConstraint;
        if (mFlags.mHaveIntrinsicImageSize) {
          float width = mIntrinsicImageSize.width
            ? (float) mIntrinsicImageSize.width
            : (float) mIntrinsicImageSize.height;     // avoid divide by zero
          float height = (float) mIntrinsicImageSize.height;
          newHeight = (nscoord)
            NSToIntRound(widthConstraint * height / width);
          haveComputedSize = PR_TRUE;
        }
        else {
          newHeight = 1;
          needIntrinsicImageSize = PR_TRUE;
          mFlags.mNeedSizeNotification = PR_TRUE;
        }
      }
    }
    else if (fixedContentHeight) {
      // We have a height, and an auto width. Compute width from height
      // once we have the intrinsic image size.
      newHeight = heightConstraint;
      if (mFlags.mHaveIntrinsicImageSize) {
        float width = (float) mIntrinsicImageSize.width;
        float height = mIntrinsicImageSize.height
          ? (float) mIntrinsicImageSize.height
          : (float) mIntrinsicImageSize.width;         // avoid divide by zero
        newWidth = (nscoord)
          NSToIntRound(heightConstraint * width / height);
        haveComputedSize = PR_TRUE;
      }
      else {
        newWidth = 1;
        needIntrinsicImageSize = PR_TRUE;
        mFlags.mNeedSizeNotification = PR_TRUE;
      }
    }
    else {
      mFlags.mAutoImageSize = PR_TRUE;
      if (mFlags.mHaveIntrinsicImageSize) {
        newWidth = mIntrinsicImageSize.width;
        newHeight = mIntrinsicImageSize.height;
        haveComputedSize = PR_TRUE;
      }
      else {
        newWidth = 1;
        newHeight = 1;
        needIntrinsicImageSize = PR_TRUE;
        mFlags.mNeedSizeNotification = PR_TRUE;
      }
    }
    mFlags.mNeedIntrinsicImageSize = needIntrinsicImageSize;
    mFlags.mHaveComputedSize = haveComputedSize;
    mComputedImageSize.width = newWidth;
    mComputedImageSize.height = newHeight;
#ifdef NOISY_IMAGE_LOADING
    nsFrame::ListTag(stdout, mFrame);
    printf(": %s%scomputedSize=%d,%d\n",
           mFlags.mNeedIntrinsicImageSize ? "need-instrinsic-size " : "",
           mFlags.mHaveComputedSize ? "have-computed-size " : "",
           mComputedImageSize.width, mComputedImageSize.height);
#endif

    // Load the image at the desired size
    if ((0 != newWidth) && (0 != newHeight)) {
      // Make sure we squelch a callback to the client of this image
      // loader during a start-load-image. Its possible the image we
      // want is ready to go and will therefore fire a notification
      // during the StartLoadImage call. Since this routine is already
      // returning size information there is no point in passing on the
      // callbacks to the client.
      mFlags.mSquelchCallback = PR_TRUE;
      StartLoadImage(aPresContext);
      mFlags.mSquelchCallback = PR_FALSE;

      // See if we just got the intrinsic size
      if (mFlags.mNeedIntrinsicImageSize && mFlags.mHaveIntrinsicImageSize) {
        // We just learned our intrinisic size. Start over from the top...
#ifdef NOISY_IMAGE_LOADING
        printf("  *** size arrived during StartLoadImage, looping...\n");
#endif
        continue;
      }
    }
    break;
  }

  aDesiredSize.width = mComputedImageSize.width;
  aDesiredSize.height = mComputedImageSize.height;

  if ((mFlags.mNeedIntrinsicImageSize && !mFlags.mHaveIntrinsicImageSize) ||
      mFlags.mNeedSizeNotification) {
    return PR_FALSE;
  }
  return PR_TRUE;
}

PRBool
nsHTMLImageLoader::GetLoadImageFailed() const
{
  PRBool  result = PR_FALSE;

  if (nsnull != mImageLoader) {
    // Ask the image loader whether the load failed
    PRUint32  loadStatus;
    mImageLoader->GetImageLoadStatus(&loadStatus);
    result = 0 != (loadStatus & NS_IMAGE_LOAD_STATUS_ERROR);
    return result;
  }

  result = mFlags.mLoadImageFailed ? PR_TRUE : PR_FALSE;
  return result;
}

PRUint32
nsHTMLImageLoader::GetLoadStatus() const
{
  PRUint32 loadStatus = NS_IMAGE_LOAD_STATUS_NONE;
  if (mImageLoader) {
    mImageLoader->GetImageLoadStatus(&loadStatus);
  }
  return loadStatus;
}

#ifdef DEBUG
// Note: this doesn't factor in:
// -- the mBaseURL (it might be shared)
// -- the mFrame (that will be counted elsewhere most likely)
// -- the mClosure (we don't know what type it is)
// -- the mImageLoader (it might be shared)
void
nsHTMLImageLoader::SizeOf(nsISizeOfHandler* aHandler, PRUint32* aResult) const
{
  PRUint32 sum = sizeof(*this) - sizeof(mURLSpec) - sizeof(mURL);
  PRUint32 ss;
  mURLSpec.SizeOf(aHandler, &ss);       sum += ss;
  mURL.SizeOf(aHandler, &ss);           sum += ss;
  *aResult = sum;
}
#endif
