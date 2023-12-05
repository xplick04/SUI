import numpy as np

arr = np.random.rand(1, 3, 1, 4)
print("Original array shape:", arr.shape)

arr_squeezed = arr.squeeze()
print("Squeezed array shape:", arr_squeezed.shape)