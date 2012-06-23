using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.Win32;

namespace Ramdisk
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private bool hasExist(char ch, String[] logDrivers)
        {
            for (int i = 0; i < logDrivers.Length; i++)
            {
                string s = ":\\";
                s = ch + s;
                if (s.Equals(logDrivers[i]))
                    return true;
            }
            return false;
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            string registData;
            RegistryKey hkml = Registry.LocalMachine;
            RegistryKey system = hkml.OpenSubKey("SYSTEM", true);
            RegistryKey currentcontrolset = system.OpenSubKey("CurrentControlSet", true);
            RegistryKey services = currentcontrolset.OpenSubKey("services", true);
            RegistryKey ramdisk = services.OpenSubKey("Ramdisk", true);
            RegistryKey parameters = ramdisk.OpenSubKey("Parameters", true);
            registData = parameters.GetValue("DiskSize").ToString();
            this.barSize.Value = int.Parse(registData) / (1000 * 1000);
            label3.Text = this.barSize.Value.ToString() + "MB";
            registData = parameters.GetValue("DriveLetter").ToString();
            

            String[] logDrivers = System.IO.Directory.GetLogicalDrives();

            for (char ch = 'C'; ch <= 'Z'; ch++)
            {
                if (hasExist(ch, logDrivers)) continue;
                string add = ":";
                add = ch + add;
                choice.Items.Add(add);
            }
            choice.Text = registData;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string registData;
            RegistryKey hkml = Registry.LocalMachine;
            RegistryKey system = hkml.OpenSubKey("SYSTEM", true);
            RegistryKey currentcontrolset = system.OpenSubKey("CurrentControlSet", true);
            RegistryKey services = currentcontrolset.OpenSubKey("services", true);
            RegistryKey ramdisk = services.OpenSubKey("Ramdisk", true);
            RegistryKey parameters = ramdisk.OpenSubKey("Parameters", true);

            registData = (this.barSize.Value*1000*1000).ToString();
            label3.Text = this.barSize.Value.ToString() + "MB";
            parameters.SetValue("DiskSize", int.Parse(registData));
            registData = choice.Text;
            parameters.SetValue("DriveLetter", registData);

        }

        private void button2_Click(object sender, EventArgs e)
        {
            Process.Start("shutdown", "-r");    
        }

        private void barSize_Scroll(object sender, EventArgs e)
        {
            label3.Text = this.barSize.Value.ToString() + "MB";
        }
    }
}
