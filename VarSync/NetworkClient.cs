using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace VarSync
{
    class NetworkClient
    {
        System.Net.Sockets.TcpClient tcpClient;
        bool isConnected = false;

        public NetworkClient()
        {
            tcpClient = new System.Net.Sockets.TcpClient();
        }

        public void TryConnect(string hostname, int port)
        {
            tcpClient.Connect(hostname, port);
            isConnected = true;
        }

        public bool IsConnected {  get { return isConnected; } }

        public void Send(NetworkStream networkStream, Variable v)
        {
            var msg = Encoding.ASCII.GetBytes(v.ToString());
            networkStream.Write(msg, 0, msg.Length);
        }

        public void SendAll(List<Variable> vars)
        {
            var stream = tcpClient.GetStream();
            foreach (Variable v in vars)
            {
                Send(stream, v);
            }
        }

        internal void Close()
        {
            if (isConnected)
            {
                tcpClient.Close();
            }
        }

    }
}
