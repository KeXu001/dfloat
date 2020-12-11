
import argparse
import numpy as np

def positive_int(x_str):
  x = int(x_str)
  if x > 0:
    return x
  else:
    raise argparse.ArgumentTypeError('Should be psitive')

def gen_random_numbers(n, cap):
  return np.random.randint(low=1, high=cap, size=n)

def save_to_file(numbers, outfile):
  with open(outfile, 'w') as f:
    numbers.tofile(f)
  return

def main(args):
  numbers = gen_random_numbers(args.n, args.cap)
  save_to_file(numbers, args.outfile)
  return

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('n', type=positive_int, help='How many numbers to generate')
  parser.add_argument('cap', type=positive_int, help='Maximum possible value')
  parser.add_argument('outfile', type=str, help='Where to save binary data (8 bytes per number)')
  args = parser.parse_args()

  main(args)
