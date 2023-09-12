import random
import sys

def generate_random_int(low, high):
    return random.randint(low, high)

if len(sys.argv) != 3:
    print("Usage: python random_integer.py <low> <high>")
    sys.exit(1)

low_value = int(sys.argv[1])
high_value = int(sys.argv[2])

random_int = generate_random_int(low_value, high_value)
print(f"{random_int}")