/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package sendmulticast;

import java.io.IOException;
import java.net.MulticastSocket;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.util.Scanner;

/**
 *
 * @author tm490769@ad.u-bourgogne.fr
 */
public class SendMulticast {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        
        int port = 54322;
        
        MulticastSocket socket = new MulticastSocket(port);
        InetAddress groupAddress = Inet6Address.getByName("ff12::5000");
        socket.joinGroup(groupAddress);
        
        Scanner scanner = new Scanner(System.in);
        String line;
        while((line = scanner.nextLine()) != null && !line.equals("!stop")) {
            line = "[En Java] " + line + "\n";
            DatagramPacket message = new DatagramPacket(line.getBytes(), line.length(), groupAddress, 54321);
            socket.send(message);
        }
        socket.close();
    }
}
