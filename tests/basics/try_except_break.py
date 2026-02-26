# test deep unwind via break from nested try-except (22 of them)
while True:
 print(1)
 try:
  try:
   try:
    try:
     try:
      try:
       try:
        try:
         try:
          try:
           try:
            try:
             try:
              try:
               try:
                try:
                 try:
                  try:
                   try:
                    try:
                     try:
                      try:
                       print(2)
                       break
                       print(3)
                      except Exception:
                       pass
                     except Exception:
                      pass
                    except Exception:
                     pass
                   except Exception:
                    pass
                  except Exception:
                   pass
                 except Exception:
                  pass
                except Exception:
                 pass
               except Exception:
                pass
              except Exception:
               pass
             except Exception:
              pass
            except Exception:
             pass
           except Exception:
            pass
          except Exception:
           pass
         except Exception:
          pass
        except Exception:
         pass
       except Exception:
        pass
      except Exception:
       pass
     except Exception:
      pass
    except Exception:
     pass
   except Exception:
    pass
  except Exception:
   pass
 except Exception:
  pass
print(4)
