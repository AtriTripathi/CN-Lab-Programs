/*
9. Using TCP/IP sockets, write a client – server program to make the client send the file
name and to make the server send back the contents of the requested file if present.
*/

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

class Server {
    public static void main(String[] args) {
        try {
            ServerSocket serverSocket = new ServerSocket(1300);
            Socket socket = serverSocket.accept();
            System.out.println("Connection established");

            Scanner socketScanner = new Scanner(socket.getInputStream());
            String fileName = socketScanner.nextLine().trim();
            PrintStream printStream = new PrintStream(socket.getOutputStream());

            File file = new File(fileName);
            if (file.exists()) {
                Scanner fileScanner = new Scanner(file);
                while (fileScanner.hasNextLine()) {
                    printStream.println(fileScanner.nextLine());
                }
            } else {
                printStream.println("ERROR: FILE DOESN'T EXISTS");
            }
            // Keep the server alive for complete transfer of data
            System.in.read();

            socket.close();
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

class Client {
    public static void main(String[] args) {
        try {
            Socket socket = new Socket("127.0.0.1", 1300);
            Scanner socketScanner = new Scanner(socket.getInputStream());
            Scanner consoleScanner = new Scanner(System.in);

            System.out.println("Enter File Name");
            String fileName = consoleScanner.nextLine();

            PrintStream printStream = new PrintStream(socket.getOutputStream());
            printStream.println(fileName);

            while (socketScanner.hasNextLine()) {
                System.out.println(socketScanner.nextLine());
            }
            socket.close();
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }
}