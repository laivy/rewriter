USE [game]
GO

SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[account](
	[id] [bigint] IDENTITY(1,1) NOT NULL,
	[name] [nvarchar](16) NOT NULL,
	[password] [nvarchar](16) NOT NULL,
	[register_date] [datetime2](0) NOT NULL,
	[last_login_date] [datetime2](0) NULL,
 CONSTRAINT [PK_account_id] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[account] ADD  CONSTRAINT [DF_account_last_login_date]  DEFAULT (NULL) FOR [last_login_date]
GO
