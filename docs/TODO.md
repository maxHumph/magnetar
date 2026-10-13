# TODO

## Development

### Primary

- [ ] Rewrite the text-based scene declaration system to be less of an abomination.

- [ ] Ui Pipeline can't render if pbr scene isnt loaded.

- [ ] Create generic pipeline type so they can be easily activated or disabled.

- [ ] Allow ui trees to be loaded and unloaded dynamically.

- [ ] Implement UI textures.

- [ ] Implement UI updates.

- [ ] Add clickable ui elements.

- [ ] **@MAJOR** Add basic collision detection using *box/ball colliders* and *separting axis theorem*.

- [ ] **@MAJOR** Add gravity to entities with (CRigidBody).

- [ ] **@MAJOR** Implement CCode in ECS.

- [ ] **@MAJOR** Add basic lighting to the PBR pipeline.


### Secordary

- [ ] Fix `ui_calc_rects()` so that an invisible base UiRect does not need to be managed in the application code.

- [ ] Add platform layer for windows.

- [ ] **@MAJOR** Add metallic-roughness to the PBR pipeline.

### Low Priority

- [ ] Clean ui_core.c.

- [ ] Make creating UiRects less tedious.

---

## Bugs

### Major


- [ ] Segfault when resizing window when no scene is loaded.

### Minor

- [ ] The resolution given during MacOS window creation is half that of the resolution used by the renderer. (UiRects render using these dimensions until the window is manually resized).

- [ ] KHR vulkan error messages when initialising vulkan on MacOS.

- [ ] Screen tearing on MacOS.

---


---




## Completed

- [X] Scale UI with window. **01/07/2026**

- [X] **Huge GPU memory leak** during Vulkan swapchain resize. **01/07/2026**

- [X] Fix UiRect pointers becoming invalid. *Use handles instead*. **05/10/2026**

- [X] Implement UI anchoring. **06/10/2026**

- [X] UiRect not rendering when using `UI_ANCHOR_BOTTOM_BIT` or `UI_ANCHOR_RIGHT_BIT`. **06/10/2026**

- [X] Implement `vulkan_load_scene()` and `vulkan_unload_scene()`. **08/10/2026**

- [X] Allow scenes to be loaded and unloaded dynamically. **08/10/2026**

- [X] When unloading scenes `vkDestroy()` is called on VkBuffers currently in use. *Add a fence to wait for idle at the start of `vulkan_backend_unload_scene`.* **12/10/2026**

- [X] Move UI resizing from shader to CPU. **12/10/2020**

- [X] Rebuild UI tree on window resize. **12/10/2026**

- [X] UiRects won't render if their extent is larger that their parent. (They should shrink down to their parents dimensions). **13/10/2026**
