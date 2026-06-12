/**
 * @file platform_macos_types.h
 */

#pragma once

#if defined(MPLATFORM_APPLE)

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

typedef struct InternalState {
  NSWindow* ns_window;
  NSView* ns_view;
  CAMetalLayer* metal_layer;
  id window_delegate;
} InternalState;

#endif
