

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
  <head>
        <meta charset="UTF-8">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="theme-color" content="#1a5ca9">
	  <meta name="msapplication-navbutton-color" content="#1a5ca9">
	  <meta name="msapplication-starturl" content="/">
	  <meta name="application-name" content="ESP32 Gate Sensor">
    <meta name="description" content="ESP32 Gate Sensor">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="preload" fetchpriority="high" as="image" type="image/x-icon" href="/favicon.ico">    
    <title>GateDisplay</title>
    <style>
 :root {
        --logs-header: #333;
      }

      h2 {font-family: Verdana, Geneva, sans-serif; font-size: 3.0rem; vertical-align: middle;}
      p {font-size: 3.0rem;}
      body {font-family: Verdana, Geneva, sans-serif; max-width: 920px; margin:0px auto; padding-bottom: 25px;}
     
      .card { background: white; padding: 30px; border-radius: 8px; box-shadow: 0 8px 20px rgba(0,0,0,0.15); width: auto; max-width: 1080px; }
      
      .switch {position: relative; display: inline-block; width: 120px; height: 68px; } 
      .switch input {display: none;}
      
      .slider {
        -webkit-appearance: none;
        position: absolute; 
        top: 0; 
        left: 0; 
        right: 0; 
        bottom: 0; 
        background-color: #2196F3; 
        border-radius: 34px; 
        border: 2px solid #39a1f4;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
      .slider:before {
        -webkit-appearance: none;
        appearance: none;
        position: absolute; 
        content: ""; 
        height: 52px; 
        width: 52px; 
        left: 8px; 
        bottom: 14px; 
        background-color: #fff; 
        -webkit-transition: .4s; 
        transition: .4s; 
        border-radius: 68px;
        border: 2px solid #fff;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2); /* Makes it look like a button */
      }

      .Yslider {
        -webkit-appearance: none;
        position: absolute; 
        top: 0; 
        left: 0; 
        right: 0; 
        bottom: 0; 
        background-color: #F1C40F; 
        border-radius: 34px; 
        border: 2px solid #FFD700;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
      .Yslider:before {
        -webkit-appearance: none;
        appearance: none;
        position: absolute; 
        content: ""; 
        height: 52px; 
        width: 52px; 
        left: 8px; 
        bottom: 14px; 
        background-color: #fff; 
        -webkit-transition: .4s; 
        transition: .4s; 
        border-radius: 68px;
        border: 2px solid #fff;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2); /* Makes it look like a button */
      }

      input:checked+.slider {background-color: #ccc;}
      input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px);}

      input:checked+.Yslider {background-color: #ccc;}
      input:checked+.Yslider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px);}

      .refresh-button { background-color: var(--logs-header); color: white; cursor: pointer; border: none;outline: inherit; margin-right: 20px;}
      .refresh-button:hover img { opacity: 0.7; }
      
      .item-inline {
        font-family: Verdana, Geneva, sans-serif;
        display: inline-block;
        align-items: center;
        vertical-align: middle;
        margin: 5px;
        padding: 14px;
      }
      .inline-metric {
        display:flex;
        text-align: left;
        vertical-align: middle;
        box-sizing: border-box; 
        align-items: center;
      }
      
      .rounded-box {
         font-family: Verdana, Geneva, sans-serif;
         border: 2px solid #81c225;   /* Defines the border style, width, and color */
         border-radius: 12px;         /* Rounds the corners */
         background-color: #73AD21;
         width: 210px;
         height: 80px;
         color: white;
         padding: 8px;                /* Adds space between the text and the border */
         text-align: center;
        box-shadow: 0 2px 5px rgba(0,0,0,0.2); /* Makes it look like a button */
      }
      .log-container {
        display: flex;
        flex-direction: column;
        border: 1px solid #ccc;       /* Optional: for visual definition of the container */
        border-radius: 4px;           /* Rounded just a little */
        width: auto;
        height: 300px;                /* Defines the total height of the viewport */
        overflow-y: auto;             /* Allows the entire container to scroll if needed, though sticky header handles most common case */
        overflow-x: hidden;
        position: relative;           /* Needed for the sticky header to work correctly within this context */
      }

      .log-header {
        display: flex; 
        font-family: Verdana, Geneva, sans-serif;
        font-size: 14px;
        font-weight: normal;
        color: white;
        background-color: var(--logs-header);
        width: 100%%;
        align-items: center; 
        padding: 5px;
        justify-content: space-between; 
        position: sticky;             /* The key property: makes the header stick at the top of its parent container */
        top: 0; 
        z-index: 10;                  /* Ensures the header sits above the log messages when scrolling */
      }
      
      .log-footer {
        display: flex; 
        font-family: Verdana, Geneva, sans-serif;
        font-weight: normal;
        font-size: 12px;
        background-color: var(--logs-header);
        color: white;
        width: 100%%;
        padding: 5px;
        margin-top: auto;             /* Pushes the footer to the bottom */
        vertical-align: middle;
        position: sticky;             /* The key property: makes the header stick at the top of its parent container */
        bottom: 0; 
        z-index: 10;                  /* Ensures the header sits above the log messages when scrolling */
        white-space: nowrap;
      }
      .log-messages { padding: 10px; flex 1;}
      .log-messages p { margin: 0 0 5px 0; white-space: nowrap; font-size: 10px; font-family: monospace; }
      .gate-panel {
        display: flex;
        flex-wrap: wrap;                      /* Allows wrapping to new lines */
        gap: 40px;                            /* Sets 40px spacing between all items */
        justify-content: flex-start;          /* Align to the left */
        flex-direction: row;                  /* Default: main axis runs from left to right */
        padding: 20px;
        max-width: 1080px;
        margin: auto;
      }
      .card-label {
        color: #666;
      }
      .main-container {
        display: flex;
        justify-content: flex-start;
        flex-direction: column; 
        margin: 39.84px;
      }
      .header-item{
        padding 10px;
      }
      .header-container {
        font-family: Verdana, Geneva, sans-serif; 
        font-size: 3.0rem;
        font-weight: bold;
        display: flex;
        align-items: center; /* This vertically centers the items */
        gap: 10px; /* Adds space between the image and the h2 */
      }
      .favicon-icon {
        width: 46px;  /* Adjust size as needed */
        height: 46px;
      }
      .version {
        font-size: 10px;
        padding-left: 46px;
        padding-bottom: 12px;
      }
    </style>
  </head>
  <body>
    <main role="main">
    <div class="main-container">
      <div class="header-container">
        <img src="/favicon.ico" alt="Icon" class="header-item favicon-icon" width="46" height="46">
        <div class="header-item">%HOSTNAME%</div>
      </div>
      <div class="version">Version: %VERSION%</div>
    </div>
    <div class="card">
        <div class="gate-panel">
          <h1><div class="item-inline">Gate : <div class="item-inline rounded-box">%GATESTATUS%</div></div></h1>
          <h1><div class="item-inline">&nbsp;Box : <div class="item-inline rounded-box">%BOXSTATUS%</div></div></h1>
          </BR>
          <h2>&nbsp;&nbsp;Button&nbsp;<label class="item-inline switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="PressMe"><span class="item-inline slider"></span></label></div></h2>
          </BR></BR>
        </div>
        </BR>
        <div id="logcontainer" name="logcontainer" class="log-container">
          <div class="log-header"><span>&nbsp;Log Messages</span>
            <button id="refreshBtn" alt="Refresh Logs" class="refresh-button" aria-label="Refresh">
              <svg fill="#ffffff" version="1.1" id="Capa_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="18px" height="18px" viewBox="0 0 487.23 487.23" xml:space="preserve">
              <g>
	              <g>
		              <path d="M55.323,203.641c15.664,0,29.813-9.405,35.872-23.854c25.017-59.604,83.842-101.61,152.42-101.61
			          c37.797,0,72.449,12.955,100.23,34.442l-21.775,3.371c-7.438,1.153-13.224,7.054-14.232,14.512
			          c-1.01,7.454,3.008,14.686,9.867,17.768l119.746,53.872c5.249,2.357,11.33,1.904,16.168-1.205
			          c4.83-3.114,7.764-8.458,7.796-14.208l0.621-131.943c0.042-7.506-4.851-14.144-12.024-16.332
			          c-7.185-2.188-14.947,0.589-19.104,6.837l-16.505,24.805C370.398,26.778,310.1,0,243.615,0C142.806,0,56.133,61.562,19.167,149.06
			          c-5.134,12.128-3.84,26.015,3.429,36.987C29.865,197.023,42.152,203.641,55.323,203.641z"/>
		          <path d="M464.635,301.184c-7.27-10.977-19.558-17.594-32.728-17.594c-15.664,0-29.813,9.405-35.872,23.854
			          c-25.018,59.604-83.843,101.61-152.42,101.61c-37.798,0-72.45-12.955-100.232-34.442l21.776-3.369
			          c7.437-1.153,13.223-7.055,14.233-14.514c1.009-7.453-3.008-14.686-9.867-17.768L49.779,285.089
			          c-5.25-2.356-11.33-1.905-16.169,1.205c-4.829,3.114-7.764,8.458-7.795,14.207l-0.622,131.943
			          c-0.042,7.506,4.85,14.144,12.024,16.332c7.185,2.188,14.948-0.59,19.104-6.839l16.505-24.805
			          c44.004,43.32,104.303,70.098,170.788,70.098c100.811,0,187.481-61.561,224.446-149.059
			          C473.197,326.043,471.903,312.157,464.635,301.184z"/>
	          </g>
          </g>
        </svg>
      </button>
      </div>
      <div id="logs" name="logs" class="log-messages"></div>
      <div class="log-footer"><span>Heap:</span><div id="freeheap"></div><span>&nbsp;&nbsp;Blocks:</span><div id="freeblock"></div></div>
      <div hidden id="refreshlogs"></div>
    </div>  
  </div>
  </body>
  </html>
  <script>
    let userRegistered = false;
    let userToken;
    let lastHash;
    const siteHostname = window.location.origin;

    const scrollParent = document.getElementById('logcontainer');
    const messageContainer = document.getElementById('logs');
    const parser = new DOMParser();

    // Setup Listeners for User Interactions
    const btnRefresh = document.getElementById('refreshBtn');
    let btnRefreshRunning = false;
    btnRefresh.addEventListener('click', function() {
      if (btnRefreshRunning == false) {
        getLogs();
      }
    });

    const pressBtn = document.getElementById('Switch');
    pressBtn.addEventListener('change', function() {
      toggleCheckbox(pressBtn);
    });

    const autoPressBtn = document.getElementById('AutoClose');
    autoPressBtn.addEventListener('change', function() {
      toggleCheckbox(autoPressBtn);
    });


    //const headers = new Headers();    // Add other necessary headers...
    //headers.delete('Authorization');  // Delete the Authorization header
    
  function registerUser() {
    const xhr = new XMLHttpRequest();
    xhr.open("GET", siteHostname+"/register", true);
    // Set timeout in milliseconds (e.g., 5000ms = 5 seconds)
    xhr.timeout = 5000; 

    xhr.onload = function () {
        if (xhr.status >= 200 && xhr.status < 300) {
            userToken = xhr.responseText;
            userRegistered = true;
            // console.log("New Web Session Token:", userToken); // Debug
        }
    };
    // Handle what happens when the request times out
    xhr.ontimeout = function () {
        console.error("The request for /register timed out.");
    };

    xhr.send();
  }
  
  const logObserver = new MutationObserver(() => {
    // Check if the user is already at the bottom (within a 50px buffer)
    // This prevents the UI from snapping to the bottom if the user is trying to read old logs
    const threshold = 50;
    const isAtBottom = (scrollParent.scrollHeight - scrollParent.clientHeight) <= (scrollParent.scrollTop + threshold);
     if (isAtBottom) {
        // Use requestAnimationFrame to ensure the browser has painted the new log line
        // before we calculate the new scroll position
        requestAnimationFrame(() => {
            scrollParent.scrollTop = scrollParent.scrollHeight;
       });
    }
  });

  logObserver.observe(messageContainer, { 
      childList: true,      // Watch for new log lines being added
      subtree: true,        // Watch nested elements
      characterData: true   // Watch for text updates
  });

  function toggleCheckbox(element) {
    if (element.checked) {
      element.style.backgroundColor = '#ccc';
      let thelink ="" ;
      if( element.id == 'AutoClose') {
        thelink = "/pressbutton?autoclose=true";
      }
      if( element.id == 'Switch') {
        thelink = "/pressbutton";
      }
      const xhr = new XMLHttpRequest();
      xhr.open("GET", siteHostname + thelink, true);
      xhr.setRequestHeader('auth', userToken);
      xhr.timeout = 5000;
    
      xhr.onerror = function() {
        console.error('Network error occurred');
        element.checked = false;
        element.style.backgroundColor = '#2196F3';
      };

      xhr.send();
      autoOffTimer = setTimeout(() => {
            element.checked = false;
            element.style.backgroundColor = '#2196F3';
      }, 900);
    }
  }
    
  if (!!window.EventSource)
  {
    var source = new EventSource(siteHostname+'/events');
    var logsource = new EventSource(siteHostname+'/logs');

    source.addEventListener('events', function(e)
    {
      
      if (e.data.includes('RESET') == false) {
        let newHash = hashCode(e.data);
        // Dont bother to update if the data has not changed.
        if(lastHash != newHash) {
          // Split the string "25.4|62|450" into an array
          var data = e.data.split('|');

          document.getElementById('gate').innerHTML = data[0];
          document.getElementById('gatestatus').innerHTML = data[1];
          document.getElementById('temp').innerHTML = data[2] + '<div class="cent">&nbsp;&deg;C</div>';
          document.getElementById('humidity').innerHTML = data[3] + '<div class="cent">&nbsp;&percnt;</div>';
          document.getElementById('pressure').innerHTML = data[4] + '<div class="cent">&nbsp;hPa</div>';
          document.getElementById('lastopen').innerHTML = data[5] + "&nbsp;";
          document.getElementById('lastclosed').innerHTML = data[6]+ "&nbsp;";
          document.getElementById('freeheap').innerHTML = "<b>" + formatWithCommas(data[7])+"</b> bytes";
          document.getElementById('freeblock').innerHTML = "<b>"+ formatWithCommas(data[8])+"</b> bytes";
          document.getElementById('currenttime').innerHTML = data[9];
          document.getElementById('refreshlogs').innerHTML = data[11].trim();
          lastHash = newHash;
        }
      } else {
        registerUser();
      }
    }, false);
    
    logsource.addEventListener('logs', function(e) {
      const logs = document.getElementById("logs");
      const data = e.data;
      if (logs) {
        const doc = parser.parseFromString(data, 'text/html');
        const paragraphs = doc.querySelectorAll('p');

        paragraphs.forEach((p, index) => {
          const newItem = document.createElement('p');
          newItem.textContent = p.textContent;
          logs.appendChild(newItem);
        });
      }
    }, false);
  }
  
async function getLogs() {
  btnRefreshRunning = true;
  showHourglass();
  const logs = document.getElementById("logs")
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), 5000);

  try {
    const response = await fetch("/logmessages", {
      headers: { 'auth': userToken },
      priority: 'high', // Hint to browser to move this to the front of the queue
      signal: controller.signal
    });
    
    clearTimeout(timeoutId); // Stop the timer if we succeed!

    if (!response.ok) throw new Error(`HTTP ${response.status}`);

    const text = await response.text();
    
    if (logs) {
      logs.replaceChildren(); // Faster than a while loop
      logs.innerHTML = text;
      
      // Use requestAnimationFrame for smoother scrolling
      requestAnimationFrame(() => {
        scrollParent.scrollTop = scrollParent.scrollHeight;
      });
    }
  } catch (error) {
    if (error.name === 'AbortError') {
      console.error("Fetch timed out: The server took too long to respond.");
    } else {
      console.error("Fetch failed:", error.message);
    }
  } finally {
    btnRefreshRunning = false;
    hideHourglass();
  }
}

  function showHourglass() {
    document.body.style.cursor = 'wait';
  }

  // Function to change the cursor back to normal (default)
  function hideHourglass() {
    document.body.style.cursor = 'default';  // or 'auto' to let the browser determine the cursor based on context
  }

  function formatWithCommas(number) {
    if(number) {
      return number.replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    }
    return "";
  }

  const hashCode = (str) => {
    let hash = 0;
    if(str) {
      for (let i = 0; i < str.length; i++) {
        hash = ((hash << 5) - hash) + str.charCodeAt(i);
        hash |= 0; 
      }
    }
    return hash;
  };

  </script>
)rawliteral";
