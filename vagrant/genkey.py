import os
import sys

# Size in bytes
KEY_SIZE = 16


#def get_hex(val):
#    return ''.join('{:02x}'.format(x) for x in val)


def main():
    key = os.urandom(KEY_SIZE).hex()

    #commands = f'export DRM_KEY={get_hex(key)}\n'
    #sys.stdout.write(commands)
    print(key)

if __name__ == '__main__':
    main()
