using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
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
            this.barSize.Value = int.Parse(registData) / (1024 * 1024);
            label3.Text = this.barSize.Value.ToString() + "MB";
            registData = parameters.GetValue("DriveLetter").ToString();
            this.txtDriveLetter.Text = registData;
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

            registData = (this.barSize.Value*1024*1024).ToString();
            label3.Text = this.barSize.Value.ToString() + "MB";
            parameters.SetValue("DiskSize", int.Parse(registData));
            registData = this.txtDriveLetter.Text;
            parameters.SetValue("DriveLetter", registData);

        }
    }
}
