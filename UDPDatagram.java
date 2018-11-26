/*
10. Write a program on datagram socket for client/server to display the messages on
client side, typed at the server side.
*/

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Scanner;

class UdpServer {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        try {
            DatagramSocket datagramSocket = new DatagramSocket();
            InetAddress clientAddress = InetAddress.getByName("127.0.0.1");
            String line;
            byte[] buffer;
            DatagramPacket datagramPacket;
            System.out.println("Enter Messages to Send");
            while (true) {
                line = scanner.nextLine();
                buffer = line.getBytes();
                datagramPacket = new DatagramPacket(buffer, buffer.length, clientAddress, 1234);
                datagramSocket.send(datagramPacket);
                if (line.equalsIgnoreCase("exit")) {
                    datagramSocket.close();
                    break;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class UdpClient {
    public static void main(String[] args) {
        try {
            DatagramSocket datagramSocket = new DatagramSocket(1234);
            byte[] buffer;
            DatagramPacket datagramPacket;
            System.out.println("Messages Received");
            while (true) {
                buffer = new byte[65535];
                datagramPacket = new DatagramPacket(buffer, buffer.length);
                datagramSocket.receive(datagramPacket);
                String received = new String(buffer).trim();
                System.out.println(received);
                if (received.equalsIgnoreCase("exit")) {
                    datagramSocket.close();
                    break;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}   