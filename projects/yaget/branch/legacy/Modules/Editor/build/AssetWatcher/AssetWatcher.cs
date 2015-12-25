using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;

namespace yaget
{
    public class AssetWatcher : managed.IPlugin
    {
        public AssetWatcher()
        {
            // hello here
            //vfs.fileChanged("Hello here");
            watcher = new FileSystemWatcher();
            watcher.Filter = "*.*";
            watcher.Path = "P:\\Yaget\\Repository\\Assets\\";
            watcher.IncludeSubdirectories = true;

            watcher.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName;

            watcher.Changed += new FileSystemEventHandler(watcher_Changed);
            watcher.Created += new FileSystemEventHandler(watcher_Changed);
            watcher.Deleted += new FileSystemEventHandler(watcher_Changed);

            watcher.EnableRaisingEvents = true;
        }

        void watcher_Changed(object sender, FileSystemEventArgs e)
        {
            string fullPath = e.FullPath;
            string changeType =  e.ChangeType.ToString();
            Trace.WriteLine("File: '" + fullPath + "' has changeType: '" + changeType + "'.");

            vfs.fileChanged(fullPath);
        }

        private FileSystemWatcher watcher;
    }


    public class Extra : managed.IPlugin
    {
        public Extra()
        {
        }
    }

    public class Gizmos : managed.IPlugin
    {
        public Gizmos()
        {
        }
    }

    public class ModelViewer : managed.IPlugin
    {
        public ModelViewer()
        {
        }
    }
}
