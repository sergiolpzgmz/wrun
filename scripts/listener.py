import socket, os, argparse
import sys

PORT = 8080

def create_socket():
    try:
        s = socket.socket()
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(("", PORT))
        s.listen(1)

        return s
    except:
        return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--child", action="store_true")
    args = parser.parse_args()

    s = create_socket()
    if not s:
        sys.exit()

    if args.child and os.fork() == 0:
        print(f"Child process {os.getpid()} listening")
    else:
        print(f"Process {os.getpid()} listening")

    while True:
        try:
            conn, addr = s.accept()
            conn.close()
        except:
            sys.exit()
