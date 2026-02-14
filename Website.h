const char *configHTML = R""""( 
HTTP/1.1 200 OK
Content-type:text/html
Connection: close

<!DOCTYPE html>
<html>
    <head>
        <title>ALIGMAT config</title>

        <style>
            * {
                margin: 0;
                padding: 0;
                box-sizing: border-box;
                border-radius: .5rem;
            }

            body {
                font-family:'Source Sans Pro', sans-serif;
                padding: 5rem;
                padding-bottom: 45rem;
                height: 100dvh;
                color: #575757;
            }
            
            h1 {
                font-size: 4rem;
            }
            
            h3 {
                font-size: 2rem;
                font-weight: bold;
            }

            p {
                font-size: 1.5rem;
            }

            ul {
                list-style-type: none;
                border: 2px solid black;
                gap: 2.5rem;
                padding: 2.5rem;
                margin: 0;
            }
            
            input {
                border: 2px solid black;
            }
            
            li {
                display: flex;
                justify-content: space-between;
                margin: 0.5rem 0;
            }
            
            li p {
                font-weight: 500;
                font-size: 2rem;
            }

            .delete {
                font-weight: bold;
                color: #db3f3f;
            }

            
            .list {
                display: grid;
                margin-left: auto;
                margin-right: auto;
                margin: 2rem 0;
            }

            .container {
                background-color: #F5F5F5;
                display: flex;
                align-items: center;
                justify-content: space-between;
                padding: 2rem 3rem;
                margin-bottom: 2rem;
            }
            
            .stack {
                flex-direction: column;
            }

            .stack > * {
                margin: 1.5rem 0;
                font-size: 2rem;
                width: 100%;
                background: none;
            }

            .stack ul {
                margin-bottom: 0;
            }
            
            .stack .tag {
                height: auto;
            }
            
            .btn, .pressed, input {
                padding: 1rem;
                color: #575757;
                display: flex;
                align-items: center;
                font-weight: bold;
                justify-content: center;
            }

            .btn {
                color: #F5F5F5 !important;
                background-color: #575757 !important;
            }
            .pressed {
                color: #F5F5F5 !important;
                background-color: #db3f3f !important;
            }

            #show .tag {
                display: flex;
                justify-content: space-between;
            }
            #show .tag * {
                margin-right: 1rem;
            }

            #number_list {
                gap: 2rem;
            }
        </style>
    </head>
    <body>
        <header>
            <div>
                <h1>Configuration Mode</h1>
                <p>
                    The Flood Monitoring Device is currently in configuration mode.
                </p>
            </div>
        </header>
        <div class="list">
            <div class="container" id="show">
                <div class="tag">
                    <h1>Water level:</h1>
                    <h1 id="display">pending...</h1>
                    </div>
                </div>
                <div class="container stack" id="number">
                    <div class="tag">
                    <h1>SMS Warning</h1>
                    <p>The Flood Monitoring Device can warn users through SMS.<br>Provide Phone Numbers to Enable...</p>
                </div>
                <div id="number_list" class="list">
                    <h3>No Saved Numbers...</h3>
                </div>
                <input id="input_num" placeholder="Phone Number" type="text">
                <div id="save" class="btn"><h3>Save</h3></div>
                <div id="send" class="btn"><h3>Send</h3></div>
            </div>
        </div>
        <script>
            let savedNumbers = [
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
                '0000000000',
            ];
            // let savedNumbers = ['916965106', '916965101', '916965102', '916965103', '916965104', '916965105', '916965107', '916965108', '916965109', '916965144']

            setInterval(getWaterLevel, 1000);
            setTimeout(() => {
                getNumbers();
                updateNumbers();
            }, 1000);
            
            document.getElementById("save").addEventListener("click", saveNumber);
            document.getElementById("send").addEventListener("click", sendMSG);
            document.getElementById("input_num").addEventListener("keyup", function(event) {
                if (event.key != "Enter") return;
    
                event.preventDefault();
                document.getElementById("save").click();
            })

            function getNumbers() {
                const Request = new XMLHttpRequest();
                Request.onreadystatechange = function() {
                    if (this.readyState !== XMLHttpRequest.DONE || this.status !== 200) return;
                    const displayWL = document.getElementById("display");
                    const rtext = this.responseText;
                    const data = JSON.parse(rtext.substring(rtext.indexOf("{")));
                    
                    data.numbers.forEach((e, i) => {
                        if (e.length < 10) return;
                        e = e.slice(0, 10)
                        
                        savedNumbers[i] = e;
                    })
                    updateNumbers();
                };
                
                Request.open("GET", "numbers", true);
                Request.send();
            };

            function updateNumbers() {
                const num_list = document.getElementById("number_list");
                let entry = "";
                for (let i = 0; i < savedNumbers.length; i++) {
                    if (savedNumbers[i].startsWith("0")) continue;
                    
                    entry += `<li><p>09${savedNumbers[i].slice(1)}</p><p class="delete" onclick="deleteNumber(${i})">Delete</p></li>`;
                }
                
                if (entry.length > 0) {
                    num_list.innerHTML = `<h3>Saved Numbers:</h3><ul class="list">${entry}</ul>`
                } else {
                    num_list.innerHTML = "<h3>No Saved Numbers...</h3>"
                }
            }
            
            function deleteNumber(index) {
                let coord = (index != 9) ? '0' : ''
                coord += index+1

                const sendRequest = new XMLHttpRequest();
                sendRequest.onreadystatechange = function() {
                    if (this.readyState !== XMLHttpRequest.DONE || this.status !== 200) return;
                    console.log(this.responseType);

                    savedNumbers[index] = '0000000000';
                    updateNumbers();
                }
                sendRequest.open(
                    "POST", 
                    `deleteNumber?${coord}`, 
                    true
                );
                sendRequest.send();
            }

            // function sendMSG() {
            //     const sendRequest = new XMLHttpRequest();
            //     sendRequest.onreadystatechange = function() {
            //         if (this.readyState !== XMLHttpRequest.DONE || this.status !== 200) return;
            //         console.log(this.responseType);
            //     }
            //     sendRequest.open(
            //         "POST", 
            //         `message`, 
            //         true
            //     );
            //     sendRequest.send();
            // }

            function saveNumber() {
                let number = document.getElementById('input_num').value;

                if (number.startsWith("+639")) number = number.replace("+639", "");
                else if (number.startsWith("09")) number = number.slice(2);
                
                if (
                    !/^\d+$/.test(number) || 
                    number.length != 9 ||
                    savedNumbers.includes('1' + number)
                ) {
                    updateBtn("save", "Error", "Invalid Number!", "Save")
                    return
                }
                
                let coord = ''
                // savedNumbers.push('09' + number);

                for (i = 0; i < 10; i++) {
                    if (savedNumbers[i].startsWith("0")) {
                        savedNumbers[i] = '1' + number;

                        if (i+1 != 10) coord += '0';
                        coord += i+1;

                        break;
                    } else if (i == 9) {
                        updateBtn("save", "error", "Max Numbers Reached!", "Save")
                        return
                    }
                }

                updateNumbers();
                
                const sendRequest = new XMLHttpRequest();
                sendRequest.onreadystatechange = function() {
                    if (this.readyState !== XMLHttpRequest.DONE || this.status !== 200) return;
                    console.log(this.responseType);
                }
                sendRequest.open(
                    "POST", 
                    `addNumber?${number}${coord}`, 
                    true
                );
                sendRequest.send();
            };
            
            function getWaterLevel() {
                    const listRequest = new XMLHttpRequest();
                    listRequest.onreadystatechange = function() {
                        if (this.readyState !== XMLHttpRequest.DONE || this.status !== 200) return;
                    const displayWL = document.getElementById("display");
                    const rtext = this.responseText;
                    const data = JSON.parse(rtext.substring(rtext.indexOf("{")));

                    displayWL.innerHTML = data.waterLevel + " cm";
                };
                
                listRequest.open("GET", "waterLevel", true);
                listRequest.send();
            }

            function updateBtn(btnId, type, msg, lastinner) {
                const btn = document.getElementById(btnId);
                
                if (type.toLowerCase() == 'error') {
                    btn.classList.add("pressed")
                    btn.classList.remove("btn")
                    btn.innerHTML = msg
                }

                setTimeout(() => {
                    btn.classList.remove("pressed");
                    btn.classList.add("btn");
                    btn.innerHTML = lastinner
                }, 1000)
            }
        </script>
    </body>
</html>

)"""";