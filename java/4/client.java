import java.net.*;
import java.io.*;

class Client {
  public static void main(String[] args) {
    if (args.length < 2) return;
    try {
      Socket s = new Socket(args[0], Integer.parseInt(args[1]));
      BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
      while (true) System.out.println(in.readLine());
    } catch (UnknownHostException e) {
      System.out.println("Unknown host.");
      System.exit(1);
    } catch (IOException e) {
      System.out.println("IO exception.");
      System.exit(1);
    }
  }
}
