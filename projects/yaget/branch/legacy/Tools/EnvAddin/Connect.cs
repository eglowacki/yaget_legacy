using System;
using Extensibility;
using EnvDTE;
using EnvDTE80;
using System.IO;
using System.Diagnostics;
using Microsoft.VisualStudio.VCProjectEngine;
using VSLangProj;
using System.Collections.Generic;

namespace yaget
{
	/// <summary>The object for implementing an Add-in.</summary>
	/// <seealso class='IDTExtensibility2' />
	public class Connect : IDTExtensibility2
	{
		/// <summary>Implements the constructor for the Add-in object. Place your initialization code within this method.</summary>
		public Connect()
		{
		}

		/// <summary>Implements the OnConnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being loaded.</summary>
		/// <param term='application'>Root object of the host application.</param>
		/// <param term='connectMode'>Describes how the Add-in is being loaded.</param>
		/// <param term='addInInst'>Object representing this Add-in.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnConnection(object application, ext_ConnectMode connectMode, object addInInst, ref Array custom)
		{
			_applicationObject = (DTE2)application;
			_addInInstance = (AddIn)addInInst;

            if (connectMode != ext_ConnectMode.ext_cm_CommandLine)
            {
                Windows w = this._applicationObject.Windows;
                Window ww = w.Item(Constants.vsWindowKindOutput);
                OutputWindow o = (OutputWindow)ww.Object;
                OutputWindowPanes oo = o.OutputWindowPanes;

                this.outputPane = oo.Add("Build Environment");
            }

            print("Copyright Edgar Glowacki 2008 (c)");
            print("Yaget Build Environment addin loaded.");

            this._applicationObject.Events.SolutionEvents.Opened += new _dispSolutionEvents_OpenedEventHandler(SolutionEvents_Opened);
		}


        private void print(string messageText)
        {
            if (this.outputPane != null)
            {
                this.outputPane.OutputString(messageText + "\n");
            }
            else
            {
                System.Console.WriteLine(messageText);
            }
        }

        //! Return property sheet of specific name from configuration
        //! or null if does not exist
        private VCPropertySheet findPropertySheet(VCConfiguration configuration, string propName)
        {
            IVCCollection propertySheets = (IVCCollection)configuration.PropertySheets;
            foreach (object ps in propertySheets)
            {
                VCPropertySheet foundPropSheet = findPropertySheet((VCPropertySheet)ps, propName);
                if (foundPropSheet != null)
                {
                    return foundPropSheet;
                }
            }

            return null;
        }

        private VCPropertySheet findPropertySheet(VCPropertySheet propSheet, string propName)
        {
            string propSheetName = propSheet.Name;
            if (propSheetName == propName)
            {
                return propSheet;
            }

            IVCCollection childSheets = (IVCCollection)propSheet.PropertySheets;
            foreach (object cs in childSheets)
            {
                VCPropertySheet childSheet = (VCPropertySheet)cs;
                VCPropertySheet foundChildSheet = findPropertySheet(childSheet, propName);
                if (foundChildSheet != null)
                {
                    return foundChildSheet;
                }
            }

            return null;
        }

        private void addEnvironmentVariable(string line)
        {
            string[] keyValues = line.Split(new char[] { '=' });
            if (keyValues.Length > 1)
            {
                string key = keyValues[0];
                string value = keyValues[1];
                key = key.Trim();
                value = value.Trim();
                this.envVariables.Add(key, value);
                print("Added key " + key + " = " + value);
            }
        }


        private bool loadEnvironment()
        {
            this.envVariables.Clear();
            string solutionName = this._applicationObject.Solution.FullName;
            if (solutionName.EndsWith(".sln"))
            {
                solutionName += "env";
            }

            try
            {
                FileInfo fileInfo = new FileInfo(solutionName);
                if (fileInfo.Exists)
                {
                    using (StreamReader sr = new StreamReader(solutionName))
                    {
                        print("Loaded " + solutionName);
                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            line = line.Trim();
                            if (line.StartsWith(";"))
                            {
                                continue;
                            }
                            addEnvironmentVariable(line);
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                this.envVariables.Clear();
                print("Could not load  " + solutionName + ".");
                print("Error:" + e.Message + "\n" + "StackTrace:\n" + e.StackTrace);
            }

            return this.envVariables.Count > 0;
        }


        void SolutionEvents_Opened()
        {
            if (loadEnvironment())
            {
                try
                {
                    Projects solutionProjects = this._applicationObject.Solution.Projects;
                    foreach (Project baseProject in solutionProjects)
                    {
                        VCProject project = null;
                        if (baseProject.Object is VCProject)
                        {
                            project = (VCProject)baseProject.Object;
                            string projectName = project.Name;
                            IVCCollection configurationCollection = (IVCCollection)project.Configurations;
                            foreach (object c in configurationCollection)
                            {
                                VCConfiguration configuration = (VCConfiguration)c;
                                VCPropertySheet propSheet = findPropertySheet(configuration, "Library");
                                if (propSheet != null)
                                {
                                    //bool bMacroSet = false;
                                    foreach (KeyValuePair<string, string> kvp in this.envVariables)
                                    {
                                        VCUserMacro runFolderUserMacro = (VCUserMacro)(((IVCCollection)propSheet.UserMacros).Item(kvp.Key));
                                        if (runFolderUserMacro != null)
                                        {
                                            /*
                                            if (!bMacroSet)
                                            {
                                                print("---------- Setting new macros ----------");
                                            }                                

                                            bMacroSet = true;
                                            string oldValue = runFolderUserMacro.Value;
                                            */
                                            runFolderUserMacro.Value = kvp.Value;
                                            printExpendMacros(project);
                                            return;
                                            //string newValue = configuration.Evaluate(kvp.Value);
                                            //print("SET " + kvp.Key + " = " + kvp.Value);
                                        }
                                    }

                                    /*
                                    if (bMacroSet)
                                    {
                                        print("----------------- Done -----------------");
                                        return;
                                    }
                                    */
                                }
                            }
                        }
                    }
                }
                catch (System.Exception e)
                {
                    print("Could not set macros. Build Environment is in unstable state. Unloading/reloading addin suggested.");
                    print("Error:" + e.Message + "\n" + "StackTrace:\n" + e.StackTrace);
                }
            }
        }

        private void printExpendMacros(VCProject project)
        {
            IVCCollection configurationCollection = (IVCCollection)project.Configurations;
            foreach (object c in configurationCollection)
            {
                VCConfiguration configuration = (VCConfiguration)c;
                print("---------- Setting new macros for " + configuration.Name + " configuration ----------");
                foreach (KeyValuePair<string, string> kvp in this.envVariables)
                {
                    string newValue = configuration.Evaluate(kvp.Value);
                    print("SET " + kvp.Key + " = " + newValue);
                }
                print("----------------- Done -----------------");
            }
        }

		/// <summary>Implements the OnDisconnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being unloaded.</summary>
		/// <param term='disconnectMode'>Describes how the Add-in is being unloaded.</param>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnDisconnection(ext_DisconnectMode disconnectMode, ref Array custom)
		{
		}

		/// <summary>Implements the OnAddInsUpdate method of the IDTExtensibility2 interface. Receives notification when the collection of Add-ins has changed.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />		
		public void OnAddInsUpdate(ref Array custom)
		{
		}

		/// <summary>Implements the OnStartupComplete method of the IDTExtensibility2 interface. Receives notification that the host application has completed loading.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnStartupComplete(ref Array custom)
		{
		}

		/// <summary>Implements the OnBeginShutdown method of the IDTExtensibility2 interface. Receives notification that the host application is being unloaded.</summary>
		/// <param term='custom'>Array of parameters that are host application specific.</param>
		/// <seealso class='IDTExtensibility2' />
		public void OnBeginShutdown(ref Array custom)
		{
		}

        private DTE2 _applicationObject;
		private AddIn _addInInstance;
        private Dictionary<string, string> envVariables = new Dictionary<string, string>();
        private OutputWindowPane outputPane;
	}
}