import random
import sys

def generate_random_float(low, high):
    return random.uniform(low, high)


if len(sys.argv) != 3:
    print("Usage: python random_float.py <low> <high>")
    sys.exit(1)

low_value = float(sys.argv[1])
high_value = float(sys.argv[2])

random_float = generate_random_float(low_value, high_value)
print(f"{random_float}")