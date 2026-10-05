# TODO

## Development

### Primary

- [ ] Move UI resizing from shader to CPU.

- [ ] Implement UI anchoring.

- [ ] Rebuild UI tree on window resize.

- [ ] Allow scenes to be loaded and unloaded dynamically.

- [ ] Allow ui trees to be loaded and unloaded dynamically.


- [ ] Implement UI textures.

- [ ] Implement UI updates.

- [ ] Add clickable ui elements.

- [ ] **@MAJOR** Add basic collision detection using *box colliders* and *separting axis theorem*.

- [ ] **@MAJOR** Add gravity to entities with (CRigidBody).

- [ ] **@MAJOR** Implement CCode in ECS.

- [ ] **@MAJOR** Add basic lighting to the PBR pipeline.

- [ ] **@MAJOR** Add metallic-roughness to the PBR pipeline.

### Secordary

- [ ] Add platform layer for windows.

---

## Bugs

### Major

- [ ] Fix UiRect pointers becoming invalid. *Use handles instead*.

### Minor

- [ ] KHR vulkan error messages when initialising vulkan on MacOS.

- [ ] Screen tearing on MacOS.

---

## Completed

- [X] Scale UI with window. **01/07/2026**

- [X] **Huge GPU memory leak** during Vulkan swapchain resize. **01/07/2026**
