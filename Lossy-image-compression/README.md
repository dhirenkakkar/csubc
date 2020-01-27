# Lossy-image-compression

An attempt at a simple lossy image compression by breaking the image into nodes of a quad tree recursively until each leaf node contains only a pixel and finally striping out the *desired leaves*. 

#
- The algorith breaks the image into nodes of a quad tree recursively until reaching a single pixel in the leaf nodes. 
- Based on the 'entropy' calculations, trimming of pixel nodes is done so as to maintain the overall structure yet reduce the image size.
- Hence, it maintains the image structure, however makes it blocky.