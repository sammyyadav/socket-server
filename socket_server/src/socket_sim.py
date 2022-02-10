#!/usr/bin/env python
import rospy 
import socket
import select
import sys
import threading

from std_msgs.msg import String
from geometry_msgs.msg import Twist


rospy.init_node('socket')

hostname = socket.gethostname()
local_ip = socket.gethostbyname(hostname)

def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.
    print(local_ip)


list_of_clients = []


def clientthread(conn, addr):
    # sends a message to the client whose user object is conn
    while not rospy.is_shutdown():
        try:

            message = conn.recv(2048)
            velocity_publisher = rospy.Publisher('/cmd_vel', Twist, queue_size=10)
            vel_msg = Twist()
            vel_msg.linear.x = 0
            vel_msg.linear.y =  0
            vel_msg.linear.z = 0
            vel_msg.angular.x = 0
            vel_msg.angular.y =0
            vel_msg.angular.z = 0

            
            if message:
                """prints the message and address of the
                user who just sent the message on the server
                terminal"""
                print(str(message.decode('utf-8')))
                if str(message.decode('utf-8')) == "s":
                    vel_msg.linear.x = 0
                    vel_msg.angular.z = 0
                    
                    
                elif str(message.decode('utf-8')) == "f":
                    vel_msg.linear.x = 1
                    vel_msg.angular.z = 0
                elif str(message.decode('utf-8')) == "b":
                    vel_msg.linear.x = -1
                    vel_msg.angular.z = 0
                elif str(message.decode('utf-8')) == "l":
                    vel_msg.linear.x = 0
                    vel_msg.angular.z = 1
                elif str(message.decode('utf-8')) == "r":
                    vel_msg.linear.x = 0
                    vel_msg.angular.z = -1
                
                pub = rospy.Publisher('recv_msg', String, queue_size=10)
                pub.publish(str(message.decode('utf-8')))
                velocity_publisher.publish(vel_msg) 


            else:
                """message may have no content if the connection
                is broken, in this case we remove the connection"""
                remove(conn)


        except socket.error as error:
            #print("Except")
            #print(error.message)
            continue


"""Using the below function, we broadcast the message to all
clients who's object is not the same as the one sending
the message """


def broadcast(message, connection):
    for clients in list_of_clients:
        if clients != connection:
            try:
                clients.send(message)
            except:
                clients.close()

                # if the link is broken, we remove the client
                remove(clients)


"""The following function simply removes the object
from the list that was created at the beginning of
the program"""


def remove(connection):
    if connection in list_of_clients:
        list_of_clients.remove(connection)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print_hi('PyCharm')

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('socket has been created')
s.bind(('', 9134))
s.listen(5)
print('Waiting for connections')
while True:
    """Accepts a connection request and stores two parameters,
    conn which is a socket object for that user, and addr
    which contains the IP address of the client that just
    connected"""
    conn, addr = s.accept()

    """Maintains a list of clients for ease of broadcasting
    a message to all available people in the chatroom"""
    list_of_clients.append(conn)

    # prints the address of the user that just connected
    print(addr[0] + " connected")
    conn.send(bytes('c', 'utf-8'))
    # creates and individual thread for every user
    # that connects
    t1 = threading.Thread(target=clientthread, kwargs={ "conn": conn, "addr" : addr})
    t1.start()

conn.close()
server.close()
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
