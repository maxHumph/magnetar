# Notes

## .scenebin file structure

**HEADER**

1. OFFSET 0 <signature>
2. OFFSET 4 <number-of-each-object>
3. OFFSET prev + 32 * UNIQUE OBJECT TYPES <object-arrays>
4. OFFSET ??? <mesh-asset>
5. OFFSET ??? <mesh-asset-paths>
4. OFFSET ??? <texture-asset>
5. OFFSET ??? <texture-asset-paths>

