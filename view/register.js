window.addEventListener('load', () => init());

function init()
{
	register.addEventListener("submit", (event) => registerUser(event));
}

async function registerUser(event)
{
	event.preventDefault();
	let formData = new URLSearchParams(new FormData(document.getElementById("register")));

	let token = Math.random().toString(36).substr(2);
	token += token;

	let url = "/register";
	let data =
	{
		"register": "",
		"name": formData.get("name"),
		"username": formData.get("username"),
		"password": formData.get("password")
	}
	
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( function(response) 
		{ 
			if(response.toString() == "OK")
			{
				window.location.href = "index.html";
			}
			else
			{
				document.getElementById("errorResponseRegister").innerHTML = response.toString();
			}
		} 
	)
	.catch(error => console.log(error));
}