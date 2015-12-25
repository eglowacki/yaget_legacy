using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;

namespace Lander
{
   /// <summary>
   /// This is the main type for your game
   /// </summary>
   public class Game : Microsoft.Xna.Framework.Game
   {
      private GraphicsDeviceManager graphics;
      private SpriteBatch spriteBatch;
      private Texture2D spriteTexture;
      private Rectangle titleSafe;
      private Ship ship;

      public Game()
      {
         this.graphics = new GraphicsDeviceManager(this);
         Content.RootDirectory = "Content";
      }

      /// <summary>
      /// Allows the game to perform any initialization it needs to before starting to run.
      /// This is where it can query for any required services and load any non-graphic
      /// related content.  Calling base.Initialize will enumerate through any components
      /// and initialize them as well.
      /// </summary>
      protected override void Initialize()
      {
         // TODO: Add your initialization logic here
         base.Initialize();
      }

      /// <summary>
      /// LoadContent will be called once per game and is the place to load
      /// all of your content.
      /// </summary>
      protected override void LoadContent()
      {
         // Create a new SpriteBatch, which can be used to draw textures.
         this.spriteBatch = new SpriteBatch(GraphicsDevice);

         // TODO: use this.Content to load your game content here
         this.spriteTexture = Content.Load<Texture2D>("placeholder");
         this.titleSafe = GetTitleSafeArea(0.8f);
         this.ship = new Ship();
      }

      /// <summary>
      /// UnloadContent will be called once per game and is the place to unload
      /// all content.
      /// </summary>
      protected override void UnloadContent()
      {
         // TODO: Unload any non ContentManager content here
      }

      /// <summary>
      /// Allows the game to run logic such as updating the world,
      /// checking for collisions, gathering input, and playing audio.
      /// </summary>
      /// <param name="gameTime">Provides a snapshot of timing values.</param>
      protected override void Update(GameTime gameTime)
      {
         // Allows the game to exit
         if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
            this.Exit();

         // TODO: Add your update logic here

         base.Update(gameTime);
      }

      /// <summary>
      /// This is called when the game should draw itself.
      /// </summary>
      /// <param name="gameTime">Provides a snapshot of timing values.</param>
      protected override void Draw(GameTime gameTime)
      {
         GraphicsDevice.Clear(Color.CornflowerBlue);

         // TODO: Add your drawing code here
         this.spriteBatch.Begin();
         Vector2 pos = new Vector2(this.titleSafe.Left, this.titleSafe.Top);
         this.spriteBatch.Draw(this.spriteTexture, pos, Color.White);
         this.spriteBatch.End();

         base.Draw(gameTime);
      }

      protected Rectangle GetTitleSafeArea(float percent)
      {
         Rectangle retval = new Rectangle(
             graphics.GraphicsDevice.Viewport.X,
             graphics.GraphicsDevice.Viewport.Y,
             graphics.GraphicsDevice.Viewport.Width,
             graphics.GraphicsDevice.Viewport.Height);
#if XBOX
            // Find Title Safe area of Xbox 360.
            float border = (1 - percent) / 2;
            retval.X = (int)(border * retval.Width);
            retval.Y = (int)(border * retval.Height);
            retval.Width = (int)(percent * retval.Width);
            retval.Height = (int)(percent * retval.Height);
            return retval;            
#else
         return retval;
#endif
      }
   }
}
