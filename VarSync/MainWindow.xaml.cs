using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace VarSync
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ObservableCollection<Variable> vars = new ObservableCollection<Variable>();
        NetworkClient networkClient;
        string openedFileName = "";

        public MainWindow()
        {
            InitializeComponent();

            lvVariables.ItemsSource = vars;

            networkClient = new NetworkClient();
        }

        void valueIsChanged(object sender, EventArgs e)
        {
            if (networkClient.IsConnected)
            {
                networkClient.SendAll(vars.ToList());
            }
        }

        private void bConnect_Click(object sender, RoutedEventArgs e)
        {
            var splitted = tbConnection.Text.Split(':');
            int port = int.Parse(splitted[1]);
            string hostname = splitted[0];

            try
            {
                networkClient.TryConnect(hostname, port);
                lConnectionStatus.Background = FindResource("OnlineBrush") as LinearGradientBrush;
                networkClient.SendAll(vars.ToList());
            }
            catch (System.Net.Sockets.SocketException)
            {
                lConnectionStatus.Content = "Conn Failed";
            }

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            tbValue.Background = Brushes.WhiteSmoke;
            tbType.Background = Brushes.WhiteSmoke;
            tbName.Background = Brushes.WhiteSmoke;

            if (ValidType(tbType.Text) == false)
            {
                tbType.Background = Brushes.Red;
                return;
            }


            if (ValidName(tbName.Text) == false || vars.ToList().Find(x => x.Name == tbName.Text) != null)
            {
                tbName.Background = Brushes.Red;
                return;
            }

            if (ValidValueByType(tbType.Text, tbValue.Text) == false)
            {
                tbValue.Background = Brushes.Red;
                return;
            }

            vars.Add(new Variable() { Name = tbName.Text, Type = tbType.Text, Value = tbValue.Text });

            tbValue.Text = "";
            tbName.Text = "";
            tbType.Focus();

            if (networkClient.IsConnected)
            {
                networkClient.SendAll(vars.ToList());
            }
        }

        private bool ValidValueByType(string type, string value)
        {
            if (type == "int")
            {
                int foo = 0;
                return int.TryParse(value, out foo);
            }
            else if (type == "float")
            {
                double foo = 0;
                return double.TryParse(value, out foo);
            }

            return true;
        }

        private bool ValidName(string p)
        {
            Regex namePattern = new Regex("[a-zA-Z_][a-zA-Z_0-9]*");
            return namePattern.IsMatch(p);
        }

        private bool ValidType(string p)
        {
            return p == "string"
                || p == "int"
                || p == "float";
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (networkClient.IsConnected)
            {
                networkClient.SendAll(vars.ToList());
            }
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Varsync Files|*.varsync|Txt Files|*.txt";

            if (ofd.ShowDialog(this) == true)
            {
                openedFileName = ofd.FileName;
                vars.Clear();
                using (var file = System.IO.File.OpenText(openedFileName))
                {
                    while (file.EndOfStream == false)
                    {
                        string line = file.ReadLine();
                        TryAddLine(line);
                    }
                }

                if (networkClient.IsConnected)
                {
                    networkClient.SendAll(vars.ToList());
                }
            }
        }

        private void TryAddLine(string line)
        {
            var splitted = line.Split(' ');

            if (splitted.Count() < 4)
                return;

            if (ValidType(splitted[0]) == false)
                return;

            if (ValidName(splitted[1]) == false)
                return;

            if (splitted[2] != "=")
                return;

            StringBuilder sb = new StringBuilder();
            for (int i = 3; i < splitted.Count(); ++i)
            {
                sb.Append(splitted[i]);
                sb.Append(" ");
            }

            var value = sb.ToString();
            if (ValidValueByType(splitted[0], value) == false)
                return;

            vars.Add(new Variable() { Type = splitted[0], Name = splitted[1], Value = value });
        }

        private void MenuItem_Click_1(object sender, RoutedEventArgs e)
        {
            if (openedFileName == "")
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Filter = "Varsync File|*.varsync|Txt File|*.txt";

                if (sfd.ShowDialog(this) == true)
                {
                    openedFileName = sfd.FileName;
                    MenuItem_Click_1(sender, e);
                }

                return;
            }

            using (var file = System.IO.File.OpenWrite(openedFileName))
            {
                foreach (var v in vars)
                {
                    var msg = Encoding.ASCII.GetBytes(String.Format("{0} {1} = {2}\n", v.Type, v.Name, v.Value));
                    file.Write(msg, 0, msg.Length);
                }
            }
        }

        private void MenuItem_Click_2(object sender, RoutedEventArgs e)
        {
            networkClient.Close();
            Close();
        }
    }
}