/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package recievemulticast;

import java.io.IOException;
import java.net.MulticastSocket;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.DatagramPacket;

/**
 *
 * @author tm490769@ad.u-bourgogne.fr
 */
public class RecieveMulticast {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {
        int port = 54321;
        
        MulticastSocket socket = new MulticastSocket(port);
        InetAddress groupAddress = Inet6Address.getByName("ff12::5000");
        socket.joinGroup(groupAddress);
        
        byte[] buf = new byte[1500];
        DatagramPacket recv = new DatagramPacket(buf, buf.length);
        while(true) {
            socket.receive(recv);
            String message = new String(buf);
            System.out.println(message);
        }
    }
}
