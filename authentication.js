/*
window.addEventListener('load', () => init());

function init()
{
	let register = document.querySelector("#register");
	let registerForm = new URLSearchParams(new FormData(document.querySelector("#register")));
	register.addEventListener("submit", (event) => registerUser(event, registerForm));
	
	let login = document.querySelector("#login");
	let loginForm = new URLSearchParams(new FormData(document.querySelector("#login")));
	login.addEventListener("submit", (event) => loginUser(event, loginForm));
	
	let logout = document.querySelector("#logout");
	logout.addEventListener("submit", (event) => logoutUser(event));
}
*/
async function registerUser(event, formData)
{
	event.preventDefault();

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
	.catch(error => console.log(error));
}

async function loggedIn()
{
	let url = "/loggedIn";
	let data =
	{
		"loggedIn": sessionStorage.getItem('token')
	}
	
	let num = 0;
	
	let response = await fetch(url, {method: 'POST',body: JSON.stringify(data)})
		.then( response => response.text() )
		.then( response => num = parseInt(response.toString()) )
		.catch(error => console.log(error));
	return num;
}

async function loginUser(event, formData)
{
	event.preventDefault();

	if((await loggedIn()) == 0)
	{	
		let token = Math.random().toString(36).substr(2);
		token += token;

		sessionStorage.setItem('token', token);

		let url = "/login";
		let data = {
			"login": token,
			"username": formData.get("username"),
			"password": formData.get("password")
		}
		const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
		.then(response => response.text())
		.catch(error => console.log(error));
	}
}

async function logoutUser(event)
{
	event.preventDefault();

	if((await loggedIn()) == 1)
	{	
		let url = "/logout";
		let data = {
			"logout": sessionStorage.getItem('token')
		}
		const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
		.then(response => response.text())
		.catch(error => console.log(error));
	}
}
