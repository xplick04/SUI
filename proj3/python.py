import numpy as np

arr = np.random.rand(1, 1)
print("Original array shape:", arr.shape)

arr_squeezed = arr.squeeze(-1)
print("Squeezed array shape:", arr_squeezed.shape)